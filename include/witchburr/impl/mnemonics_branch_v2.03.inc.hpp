
#include "enum.hpp"
void B_common(AddrOffset<26, 2> target, AbsoluteAddress absolute, LinkBit link) {
	emit<"cccccciiiiiiiiiiiiiiiiiiiiiiiial", "c", "i", "a", "l">(18, target, absolute, link);
}

void B(AddrOffset<26,2> offset) {
	B_common(offset, witchburr::AbsoluteAddress::Relative, LinkBit::NoLink);
}

void BA(AddrOffset<26, 2> label) {
	B_common(label, AbsoluteAddress::Absolute, LinkBit::NoLink);
}

void BCLR_common(BOCond bo, Imm<5> bi, BranchHint bh, LinkBit lk) {
	emit<"ccccccoooooiiiii000hhxxxxxxxxxxl", "c", "o", "i", "h", "x", "l">(Imm<6>(19),bo,bi,bh,Imm<10>(16),lk);
}

void BLR() {
	BCLR_common(BOCond::Always, 0, BranchHint::BhReturn, LinkBit::NoLink);
}

