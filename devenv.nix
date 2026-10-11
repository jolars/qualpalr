{ pkgs, ... }:

{
  packages = with pkgs; [
    bashInteractive
    autoconf
    go-task
    jq
    llvmPackages.openmp
  ];

  languages.r = {
    enable = true;
    package = pkgs.rWrapper.override {
      packages = with pkgs.rPackages; [
        devtools
        languageserver
        glmnet
        tidyverse
        usethis
        knitr
        rmarkdown
        rhub
        spelling
        covr
        maps
        rgl
        randtoolbox
        Rcpp
        cpp11
      ];
    };
  };
}
