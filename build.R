setwd("/Users/lee/GitHub/rPDZ")
library(Rcpp)
Rcpp::Rcpp.package.skeleton("rpdz.cpp")
setwd("~/GitHub/rPDZ/rpdz.cpp")
tools::package_native_routine_registration_skeleton(".", character_only = FALSE)
Rcpp::compileAttributes(verbose=TRUE)

