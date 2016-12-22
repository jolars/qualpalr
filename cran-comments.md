## Test environments
* Local Windows 10 Pro installation 10.0.14393, R 3.3.2
* Ubuntu 12.04 and 14.04, clang and gcc, R 3.3.1 and devel (on travis ci)
* Mac OS X 10.11, R 3.3.1 (on travis ci)
* Windows Server 2012 R2 x64 (build 9600), R 3.3.2, (on appveyor)
* Win-builder, devel and R 3.3.1

## R CMD check results

0 errors | 0 warnings | 1 notes

R CMD check generated the following notes:

> * checking for GNU extensions in Makefiles ... NOTE
> GNU make is a SystemRequirements.

GNU make is required by RcppParallel.

## Hot fix 

This is a hot fix. Version 0.3.0 failed to build on

* r-devel-linux-x86_64-fedora-clang
* r-devel-macos-x86_64-clang, and
* r-release-osx-x86_64-mavericks.

This was due to the inclusion of a <ext/algorithm> header and this has now been
fixed.

## Reverse dependencies

Qualpalr has no reverse dependencies.
