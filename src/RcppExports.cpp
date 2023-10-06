// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

#ifdef RCPP_USE_GLOBAL_ROSTREAM
Rcpp::Rostream<true>&  Rcpp::Rcout = Rcpp::Rcpp_cout_get();
Rcpp::Rostream<false>& Rcpp::Rcerr = Rcpp::Rcpp_cerr_get();
#endif

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
// readPDZ
NumericVector readPDZ(std::string fileName, int start, int size);
RcppExport SEXP _rPDZ_readPDZ(SEXP fileNameSEXP, SEXP startSEXP, SEXP sizeSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type fileName(fileNameSEXP);
    Rcpp::traits::input_parameter< int >::type start(startSEXP);
    Rcpp::traits::input_parameter< int >::type size(sizeSEXP);
    rcpp_result_gen = Rcpp::wrap(readPDZ(fileName, start, size));
    return rcpp_result_gen;
END_RCPP
}
// readPDZ25
std::vector<uint32_t> readPDZ25(std::string fileName);
RcppExport SEXP _rPDZ_readPDZ25(SEXP fileNameSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type fileName(fileNameSEXP);
    rcpp_result_gen = Rcpp::wrap(readPDZ25(fileName));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_rPDZ_readPDZ24", (DL_FUNC) &_rPDZ_readPDZ24, 3},
    {"_rPDZ_readPDZ", (DL_FUNC) &_rPDZ_readPDZ, 3},
    {"_rPDZ_readPDZ25", (DL_FUNC) &_rPDZ_readPDZ25, 1},
    {NULL, NULL, 0}
};

RcppExport void R_init_rPDZ(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}