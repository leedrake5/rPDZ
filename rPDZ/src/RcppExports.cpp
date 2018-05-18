// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// rcpp_hello_world
List rcpp_hello_world();
RcppExport SEXP _rPDZ_rcpp_hello_world() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(rcpp_hello_world());
    return rcpp_result_gen;
END_RCPP
}
// readPDZ24
NumericVector readPDZ24(std::string fileName, int start, int size);
RcppExport SEXP _rPDZ_readPDZ24(SEXP fileNameSEXP, SEXP startSEXP, SEXP sizeSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type fileName(fileNameSEXP);
    Rcpp::traits::input_parameter< int >::type start(startSEXP);
    Rcpp::traits::input_parameter< int >::type size(sizeSEXP);
    rcpp_result_gen = Rcpp::wrap(readPDZ24(fileName, start, size));
    return rcpp_result_gen;
END_RCPP
}
// readPDZ25
NumericVector readPDZ25(std::string fileName, int start, int size);
RcppExport SEXP _rPDZ_readPDZ25(SEXP fileNameSEXP, SEXP startSEXP, SEXP sizeSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type fileName(fileNameSEXP);
    Rcpp::traits::input_parameter< int >::type start(startSEXP);
    Rcpp::traits::input_parameter< int >::type size(sizeSEXP);
    rcpp_result_gen = Rcpp::wrap(readPDZ25(fileName, start, size));
    return rcpp_result_gen;
END_RCPP
}
// getevch24
NumericVector getevch24(std::string fileName, int start, int size);
RcppExport SEXP _rPDZ_getevch24(SEXP fileNameSEXP, SEXP startSEXP, SEXP sizeSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type fileName(fileNameSEXP);
    Rcpp::traits::input_parameter< int >::type start(startSEXP);
    Rcpp::traits::input_parameter< int >::type size(sizeSEXP);
    rcpp_result_gen = Rcpp::wrap(getevch24(fileName, start, size));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_rPDZ_rcpp_hello_world", (DL_FUNC) &_rPDZ_rcpp_hello_world, 0},
    {"_rPDZ_readPDZ24", (DL_FUNC) &_rPDZ_readPDZ24, 3},
    {"_rPDZ_readPDZ25", (DL_FUNC) &_rPDZ_readPDZ25, 3},
    {"_rPDZ_getevch24", (DL_FUNC) &_rPDZ_getevch24, 3},
    {NULL, NULL, 0}
};

RcppExport void R_init_rPDZ(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}