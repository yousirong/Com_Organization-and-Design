case 32: {
			// add
			int Z;
			R[IR.RI.rd] = ALU(R[IR.RI.rs], R[IR.RI.rt], 8, &Z);
			break; }
		case 34: {
			// sub
			int Z;
			R[IR.RI.rd] = ALU(R[IR.RI.rs], R[IR.RI.rt], 9, &Z);
			break; }
		case 36: {
			// and
			int Z;
			R[IR.RI.rd] = ALU(R[IR.RI.rs], R[IR.RI.rt], 12, &Z);
			break; }
		case 37: {
			// or
			int Z;
			R[IR.RI.rd] = ALU(R[IR.RI.rs], R[IR.RI.rt], 13, &Z);
			break; }
		case 38: {
			// xor
			int Z;
			R[IR.RI.rd] = ALU(R[IR.RI.rs], R[IR.RI.rt], 14, &Z);
			break; }
		case 39: {
			// nor
			int Z;
			R[IR.RI.rd] = ALU(R[IR.RI.rs], R[IR.RI.rt], 15, &Z);
			break; }
		case 42: {
			// slt
			int Z;
			R[IR.RI.rd] = ALU(R[IR.RI.rs], R[IR.RI.rt], 4, &Z);
			break; }