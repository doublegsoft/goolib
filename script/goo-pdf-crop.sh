export DYLD_LIBRARY_PATH=/Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/3rd/openssl-3.6.0/build/darwin/:$DYLD_LIBRARY_PATH

# rm -rf /Users/christian/Downloads/abpms/pdf_img/*
# rm -rf /Users/christian/Downloads/abpms/pdf_crop/*
# rm -rf /Users/christian/Downloads/ccbs/pdf_img/*
# rm -rf /Users/christian/Downloads/ccbs/pdf_crop/*

# build/darwin/goo-pdf-crop -d "/Users/christian/Downloads/abpms/pdf" \
# -c "[{\"x0\":0,\"y0\":0,\"x1\":100,\"y1\":100}]"

# build/darwin/goo-pdf-crop -d "/Users/christian/Downloads/ccbs/pdf" \
# -c "[{\"x0\":0,\"y0\":0,\"x1\":100,\"y1\":100}]"

/Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/3rd/xpdf-4.05/build/darwin/xpdf/pdftopng \
-f 1 -l 1 -r 300 \
/Users/christian/Downloads/000007902912_ZHT.pdf \
/Users/christian/Downloads/