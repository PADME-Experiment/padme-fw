# padme
PADME experiment software repository

# PADME Run4 Beam Monitor Tools

This branch contains the first-stage reconstruction, calibration, monitoring, and beam-quality analysis tools developed for PADME Run4 beam studies.

The branch is based on:

```bash
feature/chamber
```
date : 2026/06/03

and is intended to provide a dedicated working area for Run4 beam-monitor studies without affecting the main PADME framework branches.
Branch created by: Marco Mancini - marco.mancini@lnf.infn.it

## Overview

The material included here represents the first stage of a broader Run4 beam-analysis workflow. The current goal is to collect and preserve the main reconstruction and monitoring tools used to study the beam conditions with different PADME detectors.

The analysis is currently organized around the following detector systems:

* Target;
* PadMMe
* ECal COG;
* TMM chamber;
* combined beam-transport and beam-profile studies.

Some parts of the code are still under active development and may be reorganized in future updates.

## Directory Structure

In addition to the standard PADME framework content, this branch includes the following dedicated directories:

```text
TargetReco/
TMM_Calibration/
TMM_monitor/
PadMMe_monitor/
BeamTwiss/
```

## `TargetReco/`

This directory contains the Run4 target reconstruction and target-calibration tools.

## `TMM_Calibration/`

This directory contains the calibration workflow for the TMM chamber, with particular focus on APV equalization and charge-response corrections + Cross-talk studies

## `TMM_monitor/`

This directory contains the Run4 TMM monitoring workflow. It includes a reconstruction chain similar to the calibration directory, but oriented toward beam monitoring and production-style reconstruction.

## `PadMMe_monitor/`

This directory contains PadMMe monitoring tools.

## `BeamTwiss/`

This directory contains a first macro for studying the beam evolution using measurements from the available beam-monitoring detectors.

The current implementation combines information from:

* target;
* PadMMe;
* ECal;
* TMM.

The code is intended as a first step toward a more complete beam-transport and beam-profile analysis. It can be extended in the future to include additional information from the beamline.

## Status

This branch should be considered a first organized snapshot of the Run4 beam-monitor analysis tools.

The code is functional for the current local workflows but still requires further cleanup, validation, and documentation before being considered a fully stable analysis package.
