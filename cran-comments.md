## Test environments
* local antergos linux installation, R 3.5.1, x64
* Ubuntu Linux 16.04 LTS, R-release, GCC on rhub
* Windows Server 2008 R2 SP1, R-devel, 32/64 bit on rhub
* Debian Linux, R-devel, GCC ASAN/UBSAN on rhub
* Fedora Linux, R-devel, clang, gfortran on rhub
* macOS 10.11 El Capitan, R-release on rhub
* winbuilder release, devel (r75507) 

## R CMD check results

0 errors | 0 warnings | 2 notes

R CMD check generated the following notes:

> * checking for GNU extensions in Makefiles ... NOTE
> GNU make is a SystemRequirements.

GNU make is required by RcppParallel.

> * checking CRAN incoming feasibility ... NOTE
> Maintainer: 'Johan Larsson <mail@larssonjohan.com>'
> 
> New maintainer:
>   Johan Larsson <mail@larssonjohan.com>
> Old maintainer(s):
>   Johan Larsson <johanlarsson@outlook.com>

The email for the maintainer of this package has been changed to
mail@larssonjohan.com. An email has been sent from johanlarsson@outlook.com
to verify this.

## Sanitizer warnings

The package incurs several warnings when ran through the
GCC ASAN/UBSAN sanitizers on rhub. I believe these to be related
to the RcppParallel package
(https://cran.r-project.org/web/checks/check_results_RcppParallel.html)
and to be harmless (see https://github.com/RcppCore/RcppParallel/issues/36).

## Reverse dependencies

qualpalr has no reverse dependencies.