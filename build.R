###in R
setwd("~/GitHub/rPDZ")
library(Rcpp)
Rcpp::Rcpp.package.skeleton("rPDZ")


###put .cpp files in and edit Description
setwd("/Users/lee/GitHub/rPDZ/rPDZ")
tools::package_native_routine_registration_skeleton(".", character_only = FALSE)
Rcpp::compileAttributes(verbose=TRUE)


####in Terminal - this step creates your R package
cd "~/GitHub/rPDZ"
R CMD build rPDZ


###back in R

###Windows
devtools::build_win(pkg="~/GitHub/rPDZ/rPDZ")


###binary build
R CMD INSTALL --build rPDZ_1.6.tar.gz


###Test
install.packages("/Users/lee/GitHub/rPDZ/Source Package/rPDZ_1.6.tar.gz", type="source", repos=NULL)
