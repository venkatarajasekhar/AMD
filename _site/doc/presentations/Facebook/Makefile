.SUFFIXES: .eps .pdf
LATEX= pdflatex
BIBTEX= bibtex

TEX = amd.tex \
			header.tex \
			footer.tex \
			motivation.tex \
			time.tex \
			intro.tex \
			projects.tex

amd: amd_fixlabels

amd_fixlabels:
	@$(LATEX) amd
ifneq ($(shell grep -s -c 'Label(s) may have changed. Rerun to get' amd.log),0)
	@echo Compiling amd again to fix labels
	@$(LATEX) amd
else
	@echo " " Labels are correct
endif

clean:
	@echo "Cleaning the main directory"
	@rm -f *.pdf *.aux *.dvi *.log *.bbl *.blg figs/*.pdf 
	@rm -f *.toc *.out *.bak *.nav *.snm *.vrb
