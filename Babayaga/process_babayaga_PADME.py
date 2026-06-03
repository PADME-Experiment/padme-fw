# import uproot
# import pandas as pd
# import sys
# import numpy as np
# import os
# import pylorentz

# id = int(sys.argv[1])
# dir = str(sys.argv[2])

# df = pd.read_csv("DraftRun4.csv")
# enbeam = df[df["id"] == id].ebeam.iloc[0]
# ecms = df[df["id"] == id].ecms.iloc[0]

# gamma = enbeam/ecms
# beta = np.sqrt(1 - pow(gamma, -2))

# workfolder  = f"/home/mancinima/Babayagapadme-fw/{dir}/period"

# f = open(f"{workfolder}{id}/run/events.dat")
# c = 0
# d = 0
# n = 0

# ee, xe, ye, ze, ep, xp, yp, zp = [], [], [], [], [], [], [], []

# for l in f.readlines():
#     if "EVENT" in l:
#       n = 1
#       c = 0
#       d = 0
#       continue
#     if d == 1:
#       if c == 2:
#         d = 0
#         n = 0
#         continue
#       if c == 0:
#         ke = l.split()        
#         ee.append(float(ke[0]))
#         xe.append(float(ke[1]))
#         ye.append(float(ke[2]))
#         ze.append(float(ke[3]))
#       if c == 1:
#         kp = l.split()
#         ep.append(float(kp[0]))
#         xp.append(float(kp[1]))
#         yp.append(float(kp[2]))
#         zp.append(float(kp[3]))
#       c += 1
#       continue
#     if n == 1:
#       d = 1
#       continue

# ele = pylorentz.Momentum4(ee, xe, ye, ze).boost(0, 0, -1, beta=beta).components.T
# pos = pylorentz.Momentum4(ep, xp, yp, zp).boost(0, 0, -1, beta=beta).components.T

# #calchep example
# #Events       P1_3 [Gev]        P2_3 [Gev]        P3_1 [Gev]        P3_2 [Gev]        P3_3 [Gev]        P4_1 [Gev]        P4_2 [Gev]        P4_3 [Gev]     Q_factor   alpha_QCD  Color chains
# #1.000E+00  2.6291000000E-01  0.0000000000E+00 -6.3203490019E-03  5.0796903795E-03  1.4947607890E-01  6.3203490019E-03 -5.0796903795E-03  1.1333392110E-01| 1.000E+00  5.211E-01 

# header = f"#CalcHEP-like output from BabaYaga \n""#Type 2 -> 2+photons\n#Initial_state\nP1_3={enbeam}  P2_3=-0.000000E+00\nStrFun1=\"IDK\"\nStrFun2=\"IDK\"\n"\
# "#PROCESS   -11(E) 11(e) -> 11(e) -11(E) + photons\n"\
# "#MASSES  5.1100000000E-04 5.1100000000E-04 5.1100000000E-04 5.1100000000E-04\n"\
# "#Cross_section(Width) n.a. \n"\
# f"#Number_of_events   {len(ele)}\n"\
# "#Sum_of_weights   n.a. n.a.\n"\
# "#Events       P1_3 [Gev]        P2_3 [Gev]        P3_1 [Gev]        P3_2 [Gev]        P3_3 [Gev]        P4_1 [Gev]        P4_2 [Gev]        P4_3 [Gev]     Q_factor   alpha_QCD  Color chains"

# open(f"{workfolder}{id}/header.dat", "w").write(header)

# ones = np.ones((len(ele)))

# calchep_like = pd.DataFrame({"ev": ones, "p13": ones*enbeam, "p23": ones*0, "p31": ele[:, 1], "p32": ele[:, 2], "p33": ele[:, 3], "p41": pos[:, 1], "p42": pos[:, 2], "p43": pos[:, 3]})
# calchep_like.columns = ["ev", "p13", "p23", "p31", "p32", "p33", "p41", "p42", "p43"]

# calchep_like.to_csv(f"{workfolder}{id}/calchep_like.csv", index=None, header=False, sep=" ")

# os.system(f"cat {workfolder}{id}/header.dat {workfolder}{id}/calchep_like.csv > {workfolder}{id}/output_{id}.txt")

# f = uproot.recreate(f"{workfolder}{id}/bhabha_gen_{id}.root")

# f["events"] = {
# "e_mom": np.sqrt(ele[:, 0]**2 - (0.511e-3)**2), 
# "e_comp": ele[:, 1:], 
# "p_mom": np.sqrt(pos[:, 0]**2 - (0.511e-3)**2), 
# "p_comp": pos[:, 1:], 
# }
# f.close()

import pandas as pd
import sys
import numpy as np
import os
from array import array

import ROOT

def boost_with_root(E, px, py, pz, beta):
    """
    Return array with columns:
    E, px, py, pz

    Equivalent to:
    pylorentz.Momentum4(E, px, py, pz).boost(0, 0, -1, beta=beta).components.T
    """
    boosted = []

    for Ei, pxi, pyi, pzi in zip(E, px, py, pz):
        v = ROOT.TLorentzVector()
        v.SetPxPyPzE(pxi, pyi, pzi, Ei)

        # Boost along negative z direction
        v.Boost(0.0, 0.0, -beta)

        boosted.append([v.E(), v.Px(), v.Py(), v.Pz()])

    return np.array(boosted)


id = int(sys.argv[1])
dir = str(sys.argv[2])

df = pd.read_csv("DraftRun4.csv")
enbeam = df[df["id"] == id].ebeam.iloc[0]
ecms = df[df["id"] == id].ecms.iloc[0]

gamma = enbeam / ecms
beta = np.sqrt(1 - pow(gamma, -2))

workfolder = f"/home/mancinima/Babayagapadme-fw/{dir}/period"

f = open(f"{workfolder}{id}/run/events.dat")

c = 0
d = 0
n = 0

ee, xe, ye, ze = [], [], [], []
ep, xp, yp, zp = [], [], [], []

for l in f.readlines():
    if "EVENT" in l:
        n = 1
        c = 0
        d = 0
        continue

    if d == 1:
        if c == 2:
            d = 0
            n = 0
            continue

        if c == 0:
            ke = l.split()
            ee.append(float(ke[0]))
            xe.append(float(ke[1]))
            ye.append(float(ke[2]))
            ze.append(float(ke[3]))

        if c == 1:
            kp = l.split()
            ep.append(float(kp[0]))
            xp.append(float(kp[1]))
            yp.append(float(kp[2]))
            zp.append(float(kp[3]))

        c += 1
        continue

    if n == 1:
        d = 1
        continue

f.close()

ee = np.array(ee)
xe = np.array(xe)
ye = np.array(ye)
ze = np.array(ze)

ep = np.array(ep)
xp = np.array(xp)
yp = np.array(yp)
zp = np.array(zp)

ele = boost_with_root(ee, xe, ye, ze, beta)
pos = boost_with_root(ep, xp, yp, zp, beta)

header = (
    f"#CalcHEP-like output from BabaYaga \n"
    "#Type 2 -> 2+photons\n"
    "#Initial_state\n"
    f"P1_3={enbeam}  P2_3=-0.000000E+00\n"
    "StrFun1=\"IDK\"\n"
    "StrFun2=\"IDK\"\n"
    "#PROCESS   -11(E) 11(e) -> 11(e) -11(E) + photons\n"
    "#MASSES  5.1100000000E-04 5.1100000000E-04 5.1100000000E-04 5.1100000000E-04\n"
    "#Cross_section(Width) n.a. \n"
    f"#Number_of_events   {len(ele)}\n"
    "#Sum_of_weights   n.a. n.a.\n"
    "#Events       P1_3 [Gev]        P2_3 [Gev]        P3_1 [Gev]        P3_2 [Gev]        P3_3 [Gev]        P4_1 [Gev]        P4_2 [Gev]        P4_3 [Gev]     Q_factor   alpha_QCD  Color chains"
)

open(f"{workfolder}{id}/header.dat", "w").write(header)

ones = np.ones(len(ele))

calchep_like = pd.DataFrame({
    "ev": ones,
    "p13": ones * enbeam,
    "p23": ones * 0,
    "p31": ele[:, 1],
    "p32": ele[:, 2],
    "p33": ele[:, 3],
    "p41": pos[:, 1],
    "p42": pos[:, 2],
    "p43": pos[:, 3],
})

calchep_like.to_csv(
    f"{workfolder}{id}/calchep_like.csv",
    index=None,
    header=False,
    sep=" "
)

os.system(
    f"cat {workfolder}{id}/header.dat "
    f"{workfolder}{id}/calchep_like.csv "
    f"> {workfolder}{id}/output_{id}.txt"
)

mass_e = 0.511e-3

root_path = f"{workfolder}{id}/bhabha_gen_{id}.root"

root_file = ROOT.TFile(root_path, "RECREATE")
tree = ROOT.TTree("events", "events")

e_mom = array("d", [0.0])
p_mom = array("d", [0.0])

e_comp = ROOT.std.vector("double")()
p_comp = ROOT.std.vector("double")()

tree.Branch("e_mom", e_mom, "e_mom/D")
tree.Branch("p_mom", p_mom, "p_mom/D")
tree.Branch("e_comp", e_comp)
tree.Branch("p_comp", p_comp)

for i in range(len(ele)):
    e_E = float(ele[i, 0])
    p_E = float(pos[i, 0])

    # For massive e-/e+, store |p|, not E
    e_mom[0] = np.sqrt(max(e_E**2 - mass_e**2, 0.0))
    p_mom[0] = np.sqrt(max(p_E**2 - mass_e**2, 0.0))

    e_comp.clear()
    p_comp.clear()

    e_comp.push_back(float(ele[i, 1]))
    e_comp.push_back(float(ele[i, 2]))
    e_comp.push_back(float(ele[i, 3]))

    p_comp.push_back(float(pos[i, 1]))
    p_comp.push_back(float(pos[i, 2]))
    p_comp.push_back(float(pos[i, 3]))

    tree.Fill()

tree.Write()
root_file.Close()
