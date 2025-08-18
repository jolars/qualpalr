{
  description = "A basic flake with a shell";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
  inputs.systems.url = "github:nix-systems/default";
  inputs.flake-utils = {
    url = "github:numtide/flake-utils";
    inputs.systems.follows = "systems";
  };

  outputs =
    { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            bashInteractive
            autoconf
            go-task
            jq
            llvmPackages.openmp
            (rWrapper.override {
              packages = with rPackages; [
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
              ];
            })
          ];
        };
      }
    );
}
