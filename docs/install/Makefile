.SUFFIXES: .eps .pdf
LATEX=pdflatex
BIBTEX=bibtex

TEX=main.tex \
	footer.tex \
	header.tex \
	documentation.tex \
	troubleshooting.tex \
	download.tex \
	installation.tex

all: pdf 

html: $(TEX)
	@htlatex main "html,3,next" 
	@htlatex main "html,3,next" 

pdf: $(TEX)
	@echo Compiling main
	@$(LATEX) --shell-escape main
	@$(MAKE) fixlabels
	@$(MAKE) fixlabels

fixlabels:
ifneq ($(shell grep -s -c 'Label(s) may have changed. Rerun to' main.log),0)
	@echo Compiling main again to fix labels
	@$(LATEX) --shell-escape main
else
	@echo " " Labels are correct
endif

clean:
	@rm -f figures/*.pdf figures/*.gpl
	@echo "Cleaning the main directory"
	@rm -f main.pdf main.ps *.aux *.dvi *.log *.bbl *.blg *.toc *.out *.bak
	@rm -f main-*.svg main.4ct main.4tc main.css main.idv main.lg
	@rm -f main*.html main.xref main.tmp main-figure* main.auxlock
