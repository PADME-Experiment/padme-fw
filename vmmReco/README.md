how to use the first draft of the new vmm3-based recostruction
This codes came from the Test Beam perforde at H4 - july 2026

VMMAnalysis is needed to convert the raw data in a readable version, with the proper recast of the event counter, time ordered
the code still has issues, when large dataset is reconstructed the event_counter starts to fail --> investigate
input are raw files --> runnnn_yyyymmddhhmmss.root
the usage is simple:
> root -l
> .L VMMAnalysis.C+
> VMMAnalysis a("nnnnn_yyyymmddhhmmss") --> the run name using just the run number and the time saved
> a.Loop()

RecoEvents takes the output of VMMAnalysis and does the real reco, hough transform, tracking, charge and hit distribution
to be improved!
same logic, but input files now are Events_runnnn_yyyymmddhhmmss.root

> root -l
> .L RecoEvents.C+
> RecoEvents e("nnnnn_yyyymmddhhmmss") --> the run name using just the run number and the time saved
> e.Loop()


raw data currently stored in padmevmmdaq1 and padmevmmdaq2
or some data are in marco mancini local laptop
