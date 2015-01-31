.SUFFIXES: .eps .pdf
LATEX=pdflatex
BIBTEX=bibtex

TEX = amd.tex \
      header.tex \
      footer.tex 

all: pdf 

html: $(TEX)
	@htlatex amd "html,3,next" 
	@htlatex amd "html,3,next" 
	@cp custom.css amd.css

pdf: $(TEX)
	@echo Compiling amd
	@$(LATEX) --shell-escape amd
	@$(MAKE) fixlabels
	@$(MAKE) fixlabels

fixlabels:
ifneq ($(shell grep -s -c 'Label(s) may have changed. Rerun to' amd.log),0)
	@echo Compiling amd again to fix labels
	@$(LATEX) --shell-escape amd
else
	@echo " " Labels are correct
endif

clean:
	@rm -f figures/*.pdf figures/*.gpl
	@echo "Cleaning the amd directory"
	@rm -f amd.pdf amd.ps *.aux *.dvi *.log *.bbl *.blg *.toc *.out *.bak
	@rm -f amd-*.svg amd.4ct amd.4tc amd.css amd.idv amd.lg
	@rm -f amd*.html amd.xref amd.tmp amd-figure* amd.auxlock
	@rm -f *.toc *.out *.bak *.nav *.snm *.vrb
