# Build routine for rPDZ. Run each block from the directory it indicates.
#
# IMPORTANT: rPDZ is an existing package, NOT a new one.
# Do NOT run Rcpp::Rcpp.package.skeleton() here -- it creates a fresh
# skeleton (rcpp_hello_world only) and overwrites all of this package's
# R/, src/, NAMESPACE, and DESCRIPTION with empty stubs. That was what
# shipped the broken rPDZ_2.0.2 which only exported rcpp_hello_world().


# ---- Step 1 (R): regenerate Rcpp glue from src/rpdz.cpp ----
#
# Run the next three lines inside R (not zsh). This rewrites
# src/RcppExports.cpp, R/RcppExports.R, and src/rPDZ_init.c so every
# [[Rcpp::export]] in src/rpdz.cpp is wired up.

setwd("~/GitHub/rPDZ")
Rcpp::compileAttributes(verbose = TRUE)
tools::package_native_routine_registration_skeleton(".", character_only = FALSE)


# ---- Step 2 (Terminal): build the source tarball ----
#
#   cd ~/GitHub/rPDZ
#   R CMD build .
#   mv rPDZ_2.0.2.tar.gz "Source Package/"
#
# The tarball MUST contain src/rpdz.cpp. Verify with:
#   tar -tzf "Source Package/rPDZ_2.0.2.tar.gz" | grep src/


# ---- Step 3: install locally (pick one) ----
#
# (a) From the freshly built tarball (recommended — matches what users get):
#   R CMD INSTALL "Source Package/rPDZ_2.0.2.tar.gz"
#
# (b) Directly from the source tree (faster iteration):
#   R CMD INSTALL ~/GitHub/rPDZ


# ---- Step 4 (R): sanity check ----
#
# Confirm the installed package has ALL exports, not just rcpp_hello_world:
#
#   library(rPDZ)
#   ls("package:rPDZ")   # should list ~19 functions
#   stopifnot("getPDZDataFrames" %in% ls("package:rPDZ"))


# ---- Step 5 (optional, Windows cross-build) ----
#
# devtools::build_win(pkg = "~/GitHub/rPDZ")
#
# That produces a Windows binary .zip that you can drop into Binaries/
# and upload to the CloudCal Packages/ branch.
