# padme
PADME experiment software repository

# PADME Event Generators and ToyMC Tools

This branch contains additional tools and resources developed for PADME Monte Carlo production and event generation studies.

In addition to the standard PADME framework, three main directories are included:

## Directory Structure

### `ToyMC/`

This directory contains several PADME Toy Monte Carlo codes developed by:

**Marco Mancini**
Email: [marco.mancini@lnf.infn.it](mailto:marco.mancini@lnf.infn.it)

The current implementation includes different studies and prototype developments used during Run3, Run4 and possible future runs preparation activities.

Please note that some parts of the code may still be incomplete or under development. An improved and more organized version of the ToyMC framework is currently being developed.

---

### `Babayaga/`

This directory contains the full BabaYaga event generator setup, including:

* source code
* configuration files
* Makefile
* generated event samples for PADME Run3 studies

The README file inside the directory contains instructions and examples on how to compile and run the generator.

---

### `CalcHEP/`

This directory contains:

* CalcHEP generator package (provided as compressed archive)
* generated event samples for PADME Run3 analyses

The generated datasets were used for dedicated signal and background production campaigns.

---

## Environment Configuration

The full setup is configured through:

```bash
Configure/Marco_config.sh
```

This script defines the environment variables and software configuration needed to run the generators and ToyMC tools.

---

## Compatibility

The framework has been tested mainly on:

* CentOS7
* ROOT 5 / ROOT 6

Additional compatibility checks are still ongoing for:

* AlmaLinux 9

No major compatibility issues are currently expected, but full validation is still in progress.

---

## Notes

This branch is intended as a working area for dedicated event
generation and Monte Carlo studies within the PADME collaboration.

The code structure may evolve over time as the framework is progressively reorganized and cleaned.
