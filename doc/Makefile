NAME=ifj12

$(NAME).pdf: $(NAME).dvi
	dvipdf $(NAME).dvi

$(NAME).dvi: title.tex prectab.tex $(NAME).tex
	make -C img
	latex -halt-on-error $(NAME).tex
	latex -halt-on-error $(NAME).tex

prectab.tex: prectab.csv
	csv2latex --nohead --position c --noescape prectab.csv > prectab.tex
	sed -i 's/</\\textless/g' prectab.tex
	sed -i 's/>/\\textgreater/g' prectab.tex

open: clean $(NAME).pdf
	xdg-open $(NAME).pdf 2>/dev/null

clean:
	rm -f $(NAME).dvi $(NAME).aux $(NAME).toc $(NAME).log $(NAME).out prectab.tex

