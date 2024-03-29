###in R
setwd("~/GitHub/rPDZ")
library(Rcpp)
Rcpp::Rcpp.package.skeleton("rPDZ")


###put .cpp files in
setwd("/Users/lee/GitHub/rPDZ/rPDZ")
tools::package_native_routine_registration_skeleton(".", character_only = FALSE)
Rcpp::compileAttributes(verbose=TRUE)


####in Terminal - this step creates your R package
cd "~/GitHub/rPDZ"
R CMD build rPDZ


###back in R

###Windows
devtools::build_win(pkg="~/GitHub/rPDZ")

###binary build
R CMD INSTALL --build rPDZ_1.3.tar.gz


###Test
install.packages("/Users/lee/GitHub/rPDZ/Packages/rPDZ_1.2.tar.gz", type="source", repos=NULL)
