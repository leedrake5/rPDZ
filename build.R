setwd("/Users/lee/GitHub/rPDZ")
library(Rcpp)
Rcpp::Rcpp.package.skeleton("rPDZ")
###put .cpp files in
setwd("/Users/lee/GitHub/rPDZ/rPDZ")
tools::package_native_routine_registration_skeleton(".", character_only = FALSE)
Rcpp::compileAttributes(verbose=TRUE)

####in Terminal
cd "/Users/lee/GitHub/rPDZ"
sudo R CMD build rPDZ
