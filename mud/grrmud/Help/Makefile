


all: imm_idx mort_idx
	../help_filter/help2html imm_help.html IMM*_[1-9]
	../help_filter/help2html mort_help.html [a-z]*_[1-9]

imm_idx:
	rm -f IMM_wizhelp_1
	@echo "<help immortal wizhelp>" > IMM_wizhelp_1
	@echo "" >> IMM_wizhelp_1
	@echo "Here is a list of all immortal/builder help topics." >> IMM_wizhelp_1
	@echo "" >> IMM_wizhelp_1
	../help_filter/idx_bldr IMM*_[1-9] >> IMM_wizhelp_1
	@echo "" >> IMM_wizhelp_1
	@echo "<color=red>" >> IMM_wizhelp_1
	@echo "See Also:  commands, topics" >> IMM_wizhelp_1
	@echo "</color></help>" >> IMM_wizhelp_1


mort_idx:
	rm -f topics_1
	@echo "<help general topics>" > topics_1
	@echo "" >> topics_1
	@echo "Here is a list of all mortal help topics." >> topics_1
	@echo "" >> topics_1
	../help_filter/idx_bldr [a-z]*_[1-9] >> topics_1
	@echo "" >> topics_1
	@echo "<color=red>" >> topics_1
	@echo "See Also:  commands, wizhelp" >> topics_1
	@echo "</color></help>" >> topics_1

clean:
	rm -f imm_help.html
	rm -f mort_help.html
