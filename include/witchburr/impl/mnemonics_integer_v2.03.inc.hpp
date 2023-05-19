
void ADDI(GpReg rt, GpReg ra, Imm<16> si) {
	emit<"cccccctttttaaaaadddddddddddddddd", "c", "t", "a", "d">(Imm<6>(14), rt, ra, si);
}

void ORI(GpReg ra, GpReg rs, Imm<16> uimm) {
	emit<"oooooosssssaaaaaiiiiiiiiiiiiiiii", "o", "s", "a", "i">(Imm<6>(24), ra, rs, uimm);
}
