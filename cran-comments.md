## Test environments
* Local Windows 10 Pro installation, R release
* Ubuntu Trusty, clang and gcc, R devel, release, and oldrel (on travis ci)
* OSX El Capitan, R release and oldrel (on travis ci)
* Windows Server 2012 R2 x32 and x64, R devel, release, and oldrel (on appveyor)
* Win-builder, devel 2017-08-25 r73135

## R CMD check results

0 errors | 0 warnings | 1 notes

R CMD check generated the following notes:

> * checking for GNU extensions in Makefiles ... NOTE
> GNU make is a SystemRequirements.

GNU make is required by RcppParallel.

## Reverse dependencies

qualpalr has no reverse dependencies.
