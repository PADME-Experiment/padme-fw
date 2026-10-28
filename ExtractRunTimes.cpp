// ExtractRunTimes_fixed.cpp
// Scan every run directory and chain ALL readable .root files found directly inside it.
// Keep anomalous runs whenever a valid non-empty chain can be created.
// Output columns: RunID, detId, Unix min/max, UTC min/max.
// File suffixes are deliberately NOT used to select files:
//   run158_run_0080427_20250703_080239_1751529898_20.root
//                         ^ directory          ^ unix time ^ index
//
// Compile:
//   g++ -std=c++11 -O2 $(root-config --cflags) ExtractRunTimes.cpp -o ExtractRunTimes $(root-config --libs)
// Execute:
//   ./ExtractRunTimes [base_directory] [output_file]

#include <TChain.h>
#include <TFile.h>
#include <TTree.h>
#include <TLeaf.h>

#include <algorithm>
#include <cmath>
#include <ctime>
#include <dirent.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <regex>
#include <set>
#include <string>
#include <sys/stat.h>
#include <vector>

using namespace std;

static bool IsDirectory(const string& path)
{
    struct stat info;
    return stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
}

static bool IsRegularFile(const string& path)
{
    struct stat info;
    return stat(path.c_str(), &info) == 0 && S_ISREG(info.st_mode);
}

static bool EndsWithRoot(const string& name)
{
    return name.size() >= 5 && name.compare(name.size() - 5, 5, ".root") == 0;
}

// Return false if opendir failed: distinguish empty from unreadable directories.
static bool ListEntries(const string& directory, vector<string>& names)
{
    names.clear();
    DIR* dir = opendir(directory.c_str());
    if (!dir) {
        cerr << "ERROR: cannot open directory: " << directory << '\n';
        return false;
    }

    struct dirent* entry = nullptr;
    while ((entry = readdir(dir)) != nullptr) {
        const string name(entry->d_name);
        if (name != "." && name != "..") names.push_back(name);
    }
    closedir(dir);
    sort(names.begin(), names.end());
    return true;
}

static string UnixToUTC(double seconds)
{
    if (!isfinite(seconds)) return "INVALID_TIME";
    const time_t t = static_cast<time_t>(floor(seconds));
    struct tm utc = {};
    if (!gmtime_r(&t, &utc)) return "INVALID_TIME";

    char buffer[32];
    if (strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &utc) == 0) return "INVALID_TIME";
    return string(buffer) + " UTC";
}

// A run with multiple detector IDs is retained, with IDs comma-separated.
// Unknown filenames are retained too (detId=UNKNOWN, or e.g. 51,UNKNOWN).
static string JoinDetectorIDs(const set<string>& ids)
{
    if (ids.empty()) return "UNKNOWN";
    string result;
    for (const string& id : ids) {
        if (!result.empty()) result += ",";
        result += id;
    }
    return result;
}

int main(int argc, char** argv)
{
    const string basePath = argc > 1 ? argv[1] : "/data9Vd1/padme/leonardi/padme/daq/2025/tmmdata";
    const string outputPath = argc > 2 ? argv[2] : "run_times_2025.txt";

    if (!IsDirectory(basePath)) {
        cerr << "ERROR: base directory does not exist: " << basePath << '\n';
        return 1;
    }

    vector<string> directories;
    if (!ListEntries(basePath, directories)) return 1;

    ofstream out(outputPath.c_str());
    if (!out) {
        cerr << "ERROR: cannot create output file: " << outputPath << '\n';
        return 1;
    }
    out << "RunID\tdetId\tT_start_Unix\tT_stop_Unix\tT_start_UTC\tT_stop_UTC\n";
    out << fixed << setprecision(6);

    const regex runPattern("^run_([0-9]+)_([0-9]{8})_([0-9]{6})$");
    int processedRuns = 0;
    int skippedRuns = 0;

    for (const string& folder : directories) {
        const string runDir = basePath + "/" + folder;
        smatch folderMatch;
        if (!IsDirectory(runDir) || !regex_match(folder, folderMatch, runPattern)) continue;

        const string runID = folderMatch[1].str();
        cout << "\n=== RUN " << runID << " (" << folder << ") ===\n";
        cout << "Directory: " << runDir << '\n';

        vector<string> names;
        if (!ListEntries(runDir, names)) {
            ++skippedRuns;
            continue;
        }

        vector<string> rootFiles;
        for (const string& name : names) {
            const string path = runDir + "/" + name;
            if (EndsWithRoot(name) && IsRegularFile(path)) rootFiles.push_back(path);
        }

        // This number comes *only* from readdir(), not from a filename regex.
        cout << "ROOT files physically found in directory: " << rootFiles.size() << '\n';
        if (rootFiles.empty()) {
            cerr << "WARNING: no .root files in " << runDir << '\n';
            ++skippedRuns;
            continue;
        }
        cout << "First ROOT file: " << rootFiles.front() << '\n';
        if (rootFiles.size() > 1) cout << "Last ROOT file : " << rootFiles.back() << '\n';

        // Read every ROOT file found by readdir(); filename format does not filter files.
        // Multiple detector IDs do not cause the run to be discarded.
        const regex detPattern("^run([0-9]+)_" + folder + "_.*\\.root$");
        set<string> detectorIDs;
        TChain chain("apv_raw");
        Long64_t expectedEntries = 0;
        size_t addedFiles = 0;
        size_t unusableFiles = 0;

        for (const string& path : rootFiles) {
            const string filename = path.substr(path.find_last_of('/') + 1);
            smatch m;
            string currentID = "UNKNOWN";
            if (regex_match(filename, m, detPattern)) {
                currentID = m[1].str();
            } else {
                cerr << "WARNING: unexpected filename, including ROOT file anyway: " << filename << '\n';
            }

            Long64_t nEntries = 0;
            {
                unique_ptr<TFile> input(TFile::Open(path.c_str(), "READ"));
                if (!input || input->IsZombie()) {
                    cerr << "WARNING: cannot open ROOT file (file excluded): " << path << '\n';
                    ++unusableFiles;
                    continue;
                }
                TTree* tree = dynamic_cast<TTree*>(input->Get("apv_raw"));
                if (!tree || !tree->GetLeaf("daqTimeSec")) {
                    cerr << "WARNING: missing apv_raw/daqTimeSec (file excluded): " << path << '\n';
                    ++unusableFiles;
                    continue;
                }
                nEntries = tree->GetEntries();
                if (nEntries < 0) {
                    cerr << "WARNING: cannot read entry count (file excluded): " << path << '\n';
                    ++unusableFiles;
                    continue;
                }
            }

            // Explicitly supply known entry count, as in the working version.
            if (chain.AddFile(path.c_str(), nEntries) != 1) {
                cerr << "WARNING: TChain::AddFile failed (file excluded): " << path << '\n';
                ++unusableFiles;
                continue;
            }
            detectorIDs.insert(currentID);
            expectedEntries += nEntries;
            ++addedFiles;
        }

        const string detRunID = JoinDetectorIDs(detectorIDs);
        cout << "Detector RunID: " << detRunID
             << " | ROOT files found: " << rootFiles.size()
             << " | files added: " << addedFiles
             << " | TChain trees: " << chain.GetNtrees() << '\n';

        if (detectorIDs.size() > 1) {
            cerr << "WARNING: multiple detector run IDs for run " << runID
                 << ": " << detRunID << " (run retained)\n";
        }
        if (unusableFiles > 0) {
            cerr << "WARNING: PARTIAL RUN " << runID << ": " << unusableFiles
                 << " ROOT files not readable/usable; times refer only to "
                 << addedFiles << " files added to the chain\n";
        }

        // Skip only if no valid events can be read.
        if (addedFiles == 0 || expectedEntries == 0) {
            cerr << "WARNING: skipping run " << runID << " (no usable events)\n";
            ++skippedRuns;
            continue;
        }
        if (chain.GetNtrees() != static_cast<int>(addedFiles)) {
            cerr << "WARNING: TChain tree count mismatch for run " << runID << '\n';
        }

        const Long64_t chainEntries = chain.GetEntries();
        if (chainEntries != expectedEntries) {
            cerr << "WARNING: chain entry count mismatch for run " << runID
                 << ": chain=" << chainEntries
                 << " vs sum of files=" << expectedEntries << '\n';
        }
        if (chainEntries <= 0) {
            cerr << "WARNING: skipping run " << runID << " (empty chain)\n";
            ++skippedRuns;
            continue;
        }

        // ROOT scans all entries in the chain for these two calculations.
        const double T_start = chain.GetMinimum("daqTimeSec");
        const double T_stop  = chain.GetMaximum("daqTimeSec");

        if (!isfinite(T_start) || !isfinite(T_stop) || T_start > T_stop) {
            cerr << "ERROR: cannot determine a finite time range for run " << runID << '\n';
            ++skippedRuns;
            continue;
        }
        if (T_start < 100000000 || T_stop < 100000000) {
            cerr << "WARNING: unusual Unix timestamps for run " << runID << " (run retained)\n";
        }

        out << runID << '\t' << detRunID << '\t' << T_start << '\t' << T_stop << '\t' << UnixToUTC(T_start) << '\t' << UnixToUTC(T_stop) << '\n';
        out.flush();

        cout << "Entries: " << chainEntries << '\n'
             << "T_start: " << fixed << setprecision(6) << T_start
             << " (" << UnixToUTC(T_start) << ")\n"
             << "T_stop : " << T_stop
             << " (" << UnixToUTC(T_stop) << ")\n";
        ++processedRuns;
    }

    cout << "\nOutput: " << outputPath
         << " | processed: " << processedRuns
         << " | skipped: " << skippedRuns << '\n';
    return skippedRuns ? 2 : 0;
}
