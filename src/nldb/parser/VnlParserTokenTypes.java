// $ANTLR 2.7.7 (2006-11-01): "Vnl.g" -> "VnlLexer.java"$

/*
#  The MIT License
# 
#  Copyright 2006-2010 Karl W. Pfalzer.
#  Copyright 2012-     George P. Burdell.
# 
#  Permission is hereby granted, free of charge, to any person obtaining a copy
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is
#  furnished to do so, subject to the following conditions:
# 
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
# 
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#  THE SOFTWARE.
*/
package nldb.parser;
import  java.util.List;
import  java.util.LinkedList;
import  nldb.*;
import static nldb.Util.invariant;
import static nldb.Util.error;
import static nldb.Util.warn;

public interface VnlParserTokenTypes {
	int EOF = 1;
	int NULL_TREE_LOOKAHEAD = 3;
	int LITERAL_module = 4;
	int SEMI = 5;
	int LITERAL_endmodule = 6;
	int LPAREN = 7;
	int RPAREN = 8;
	int LITERAL_inout = 9;
	int LITERAL_input = 10;
	int LITERAL_output = 11;
	int COMMA = 12;
	int LITERAL_wire = 13;
	int LBRACK = 14;
	int RBRACK = 15;
	int POUND = 16;
	int DOT = 17;
	int IDENT = 18;
	int LCURLY = 19;
	int RCURLY = 20;
	int COLON = 21;
	int LITERAL_assign = 22;
	int ASSIGN = 23;
	int NUMBER = 24;
	int ESCAPED_IDENT = 25;
	int LITERAL_and = 26;
	int LITERAL_nand = 27;
	int LITERAL_nor = 28;
	int LITERAL_or = 29;
	int LITERAL_xor = 30;
	int LITERAL_xnor = 31;
	int SIZED_NUMBER = 32;
	int SIZE = 33;
	int BASE = 34;
	int SIZED_DIGIT = 35;
	int UNSIZED_NUMBER = 36;
	int DIGIT = 37;
	int HEXDIGIT = 38;
	int WS2 = 39;
	int WS = 40;
	int CNTRL = 41;
	int SL_COMMENT = 42;
	int ML_COMMENT = 43;
}
