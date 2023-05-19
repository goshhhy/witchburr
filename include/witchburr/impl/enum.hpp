// SPDX-FileCopyrightText: Copyright (c) 2023 linear cannon <dev@linear.network>

#pragma once

namespace witchburr {

enum class AbsoluteAddress {
	Relative = 0,
	Absolute = 1, 
};

enum class LinkBit {
	NoLink = 0,
	Link = 1,
};

enum class CrIntCond {
	LT = 0b00,
	GT = 0b01,
	EQ = 0b10,
	SO = 0b11,
};

enum class BOCond {
	DecCtrNeqZeroAndFalse 	= 0b00000,
	DecCtrEqZeroAndFalse 	= 0b00010,
	False			= 0b00100,
	DecCtrNeqZeroAndTrue	= 0b01000,
	DecCtrEqZeroAndTrue	= 0b01010,
	True			= 0b01100,
	DecCtrNeqZero		= 0b10000,
	DecCtrEqZero		= 0b10010,
	Always			= 0b10100
};

// not used for now, encoding this is a pain.
// all branch instructins will hard-encode NoHint
enum class BOHint {
	NoHint = 0b00,
	BranchOptionReserved = 0b01,
	Unlikely = 0b10,
	Likely = 0b11,
};

enum class BranchHint {
	BhReturn,
	BhNotReturn,
	BhReserved,
	BhNotPredictable,
};

}
