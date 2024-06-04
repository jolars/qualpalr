DELETE  := rm -rf
PKGNAME := $(shell sed -n "s/Package: *\([^ ]*\)/\1/p" DESCRIPTION)
PKGVERS := $(shell sed -n "s/Version: *\([^ ]*\)/\1/p" DESCRIPTION)
PKGSRC  := $(shell basename `pwd`)

CPPLIB_API_URL := https://api.github.com/repos/jolars/qualpal/releases/latest
CPPLIB_RELEASE := $(shell curl -s $(CPPLIB_API_URL) | grep "tarball_url" | cut -d '"' -f 4)

all: install

.PHONE: clean, document, compile-attributes, build, build-cran, install, clean-install, check, test, readme, vignettes, update-cpplib

clean:
	$(DELETE) src/*.o src/*.so

document: 
	Rscript -e 'devtools::document(roclets = c("rd", "collate", "namespace"))'

compile-attributes: 
	Rscript -e 'Rcpp::compileAttributes()'

build: document compile-attributes
	cd ..;\
	R CMD build --no-manual $(PKGSRC)

build-cran: compile-attributes
	cd ..;\
	R CMD build $(PKGSRC)

install: compile-attributes
	cd ..;\
	R CMD INSTALL --no-multiarch --with-keep.source $(PKGNAME)

clean-install: compile-attributes
	R CMD INSTALL --preclean --no-multiarch --with-keep.source $(PKGNAME)

check: compile-attributes
	Rscript -e 'devtools::check()'

test: compile-attributes
	Rscript -e 'devtools::test()'

readme: 
	Rscript -e 'rmarkdown::render("README.Rmd")';\
		$(DELETE) README.html

vignettes:
	Rscript -e 'devtools::build_vignettes()'

update-cpplib:
	@mkdir -p tmp
	@curl -L $(CPPLIB_RELEASE) | tar -xz --strip-components=1 -C tmp
	@rm -rf src/qualpal
	@cp -ri tmp/src/qualpal src/
	@rm -rf tmp
