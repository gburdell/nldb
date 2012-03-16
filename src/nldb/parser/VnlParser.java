// $ANTLR 2.7.7 (2006-11-01): "Vnl.g" -> "VnlParser.java"$

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

import antlr.TokenBuffer;
import antlr.TokenStreamException;
import antlr.TokenStreamIOException;
import antlr.ANTLRException;
import antlr.LLkParser;
import antlr.Token;
import antlr.TokenStream;
import antlr.RecognitionException;
import antlr.NoViableAltException;
import antlr.MismatchedTokenException;
import antlr.SemanticException;
import antlr.ParserSharedInputState;
import antlr.collections.impl.BitSet;

public class VnlParser extends antlr.LLkParser       implements VnlParserTokenTypes
 {

    private static Library stWorkLib = Library.getWorkLib();

	@Override
    public void reportError(RecognitionException ex) {
        String m = ex.getMessage();
        if (null == m) {m = "unknown parse error";}
        error("PARSE-1", ex.getFilename(), ex.getLine(), ex.getColumn(), m);
    }

	@Override
    public void reportError(String s) {
        error("PARSE-2", s);
    }

	@Override
    public void reportWarning(String s) {
		warn("PARSE-2", s);
    }

public void start() {
try {
    source_text();
}
catch (RecognitionException re) {
error("PARSE-2", re.toString());
}
catch (TokenStreamException re) {
error("PARSE-2", re.toString());  
}
}

protected VnlParser(TokenBuffer tokenBuf, int k) {
  super(tokenBuf,k);
  tokenNames = _tokenNames;
}

public VnlParser(TokenBuffer tokenBuf) {
  this(tokenBuf,2);
}

protected VnlParser(TokenStream lexer, int k) {
  super(lexer,k);
  tokenNames = _tokenNames;
}

public VnlParser(TokenStream lexer) {
  this(lexer,2);
}

public VnlParser(ParserSharedInputState state) {
  super(state,2);
  tokenNames = _tokenNames;
}

	public final void source_text() throws RecognitionException, TokenStreamException {
		
		
		try {      // for error handling
			{
			_loop3:
			do {
				if ((LA(1)==LITERAL_module)) {
					description();
				}
				else {
					break _loop3;
				}
				
			} while (true);
			}
			match(Token.EOF_TYPE);
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_0);
			} else {
			  throw ex;
			}
		}
	}
	
	public final void description() throws RecognitionException, TokenStreamException {
		
		
		try {      // for error handling
			module_declaration();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_1);
			} else {
			  throw ex;
			}
		}
	}
	
	public final void module_declaration() throws RecognitionException, TokenStreamException {
		
		String mid=null; List<String> lopd=null; Module mod=null;
		
		try {      // for error handling
			match(LITERAL_module);
			mid=module_identifier();
			{
			switch ( LA(1)) {
			case LPAREN:
			{
				lopd=list_of_ports();
				break;
			}
			case SEMI:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			match(SEMI);
			if ( inputState.guessing==0 ) {
					mod = new Module(mid,lopd);
								if (!stWorkLib.add(mod)) {
									error("DEFN-1", mid);
								}
							
			}
			{
			_loop8:
			do {
				if ((_tokenSet_2.member(LA(1)))) {
					module_item(mod);
				}
				else {
					break _loop8;
				}
				
			} while (true);
			}
			match(LITERAL_endmodule);
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_1);
			} else {
			  throw ex;
			}
		}
	}
	
	public final String  module_identifier() throws RecognitionException, TokenStreamException {
		String nm;
		
		nm=null; Token la1 = LT(1);
		
		try {      // for error handling
			{
			switch ( LA(1)) {
			case LITERAL_and:
			case LITERAL_nand:
			case LITERAL_nor:
			case LITERAL_or:
			case LITERAL_xor:
			case LITERAL_xnor:
			{
				{
				switch ( LA(1)) {
				case LITERAL_and:
				{
					match(LITERAL_and);
					break;
				}
				case LITERAL_nand:
				{
					match(LITERAL_nand);
					break;
				}
				case LITERAL_nor:
				{
					match(LITERAL_nor);
					break;
				}
				case LITERAL_or:
				{
					match(LITERAL_or);
					break;
				}
				case LITERAL_xor:
				{
					match(LITERAL_xor);
					break;
				}
				case LITERAL_xnor:
				{
					match(LITERAL_xnor);
					break;
				}
				default:
				{
					throw new NoViableAltException(LT(1), getFilename());
				}
				}
				}
				if ( inputState.guessing==0 ) {
					nm = la1.getText();
				}
				break;
			}
			case IDENT:
			case ESCAPED_IDENT:
			{
				nm=identifier();
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_3);
			} else {
			  throw ex;
			}
		}
		return nm;
	}
	
	public final List<String>  list_of_ports() throws RecognitionException, TokenStreamException {
		List<String> lop;
		
		lop = null;
		
		try {      // for error handling
			match(LPAREN);
			{
			switch ( LA(1)) {
			case IDENT:
			case ESCAPED_IDENT:
			{
				lop=list_of_port_identifiers();
				break;
			}
			case RPAREN:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			match(RPAREN);
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_4);
			} else {
			  throw ex;
			}
		}
		return lop;
	}
	
	public final void module_item(
		Module mod
	) throws RecognitionException, TokenStreamException {
		
		
		try {      // for error handling
			switch ( LA(1)) {
			case LITERAL_inout:
			case LITERAL_input:
			case LITERAL_output:
			{
				port_declaration(mod);
				break;
			}
			case IDENT:
			case ESCAPED_IDENT:
			case LITERAL_and:
			case LITERAL_nand:
			case LITERAL_nor:
			case LITERAL_or:
			case LITERAL_xor:
			case LITERAL_xnor:
			{
				module_instantiation(mod);
				break;
			}
			default:
				boolean synPredMatched13 = false;
				if (((LA(1)==LITERAL_wire||LA(1)==LITERAL_assign) && (LA(2)==IDENT||LA(2)==ESCAPED_IDENT))) {
					int _m13 = mark();
					synPredMatched13 = true;
					inputState.guessing++;
					try {
						{
						assign_statement(mod);
						}
					}
					catch (RecognitionException pe) {
						synPredMatched13 = false;
					}
					rewind(_m13);
inputState.guessing--;
				}
				if ( synPredMatched13 ) {
					assign_statement(mod);
				}
				else if ((LA(1)==LITERAL_wire) && (LA(2)==LBRACK||LA(2)==IDENT||LA(2)==ESCAPED_IDENT)) {
					net_declaration(mod);
				}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_5);
			} else {
			  throw ex;
			}
		}
	}
	
	public final List<String>  list_of_port_identifiers() throws RecognitionException, TokenStreamException {
		List<String> lopd;
		
		lopd=new LinkedList<String>(); String id=null;
		
		try {      // for error handling
			id=port_identifier();
			if ( inputState.guessing==0 ) {
				lopd.add(id);
			}
			{
			_loop19:
			do {
				if ((LA(1)==COMMA)) {
					match(COMMA);
					id=port_identifier();
					if ( inputState.guessing==0 ) {
						lopd.add(id);
					}
				}
				else {
					break _loop19;
				}
				
			} while (true);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_6);
			} else {
			  throw ex;
			}
		}
		return lopd;
	}
	
	public final void port_declaration(
		Module mod
	) throws RecognitionException, TokenStreamException {
		
		List<String> lop=null; Range rng=null; int dir=LA(1);
		
		try {      // for error handling
			{
			switch ( LA(1)) {
			case LITERAL_inout:
			{
				match(LITERAL_inout);
				break;
			}
			case LITERAL_input:
			{
				match(LITERAL_input);
				break;
			}
			case LITERAL_output:
			{
				match(LITERAL_output);
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			{
			switch ( LA(1)) {
			case LBRACK:
			{
				rng=range();
				break;
			}
			case IDENT:
			case ESCAPED_IDENT:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			lop=list_of_port_identifiers();
			if ( inputState.guessing==0 ) {
				
				Port.EDir pdir=null;
				switch (dir) {
				case VnlParserTokenTypes.LITERAL_input:
				pdir = Port.EDir.eIn; break;
				case VnlParserTokenTypes.LITERAL_inout:
				pdir = Port.EDir.eInout; break;
				case VnlParserTokenTypes.LITERAL_output:
				pdir = Port.EDir.eOut; break;
				default:
				invariant(false);
				}
				mod.portDecl(pdir,rng,lop);
				
			}
			match(SEMI);
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_5);
			} else {
			  throw ex;
			}
		}
	}
	
	public final void assign_statement(
		Module mod
	) throws RecognitionException, TokenStreamException {
		
		String id=null; Range rng=null; Expression expr=null;
		
		try {      // for error handling
			{
			switch ( LA(1)) {
			case LITERAL_wire:
			{
				match(LITERAL_wire);
				break;
			}
			case LITERAL_assign:
			{
				match(LITERAL_assign);
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			id=net_identifier();
			{
			switch ( LA(1)) {
			case LBRACK:
			{
				rng=part_select();
				break;
			}
			case ASSIGN:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			match(ASSIGN);
			expr=expression();
			match(SEMI);
			if ( inputState.guessing==0 ) {
				mod.addAssign(new Assign(id,rng,expr));
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_5);
			} else {
			  throw ex;
			}
		}
	}
	
	public final void net_declaration(
		Module mod
	) throws RecognitionException, TokenStreamException {
		
		Range rng=null; List<String> lon;
		
		try {      // for error handling
			match(LITERAL_wire);
			{
			switch ( LA(1)) {
			case LBRACK:
			{
				rng=range();
				break;
			}
			case IDENT:
			case ESCAPED_IDENT:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			lon=list_of_net_identifiers();
			match(SEMI);
			if ( inputState.guessing==0 ) {
				mod.netDecl(rng, lon);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_5);
			} else {
			  throw ex;
			}
		}
	}
	
	public final void module_instantiation(
		Module mod
	) throws RecognitionException, TokenStreamException {
		
		String ref=null;
		
		try {      // for error handling
			ref=module_identifier();
			{
			switch ( LA(1)) {
			case POUND:
			{
				parameter_instance();
				break;
			}
			case IDENT:
			case ESCAPED_IDENT:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			module_instance(mod,ref);
			{
			_loop29:
			do {
				if ((LA(1)==COMMA)) {
					match(COMMA);
					module_instance(mod,ref);
				}
				else {
					break _loop29;
				}
				
			} while (true);
			}
			match(SEMI);
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_5);
			} else {
			  throw ex;
			}
		}
	}
	
	public final Range  range() throws RecognitionException, TokenStreamException {
		Range rng;
		
		rng=null;
		
		try {      // for error handling
			match(LBRACK);
			rng=constant_range_expression();
			match(RBRACK);
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_7);
			} else {
			  throw ex;
			}
		}
		return rng;
	}
	
	public final String  port_identifier() throws RecognitionException, TokenStreamException {
		String nm;
		
		nm=null;
		
		try {      // for error handling
			nm=identifier();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_8);
			} else {
			  throw ex;
			}
		}
		return nm;
	}
	
	public final List<String>  list_of_net_identifiers() throws RecognitionException, TokenStreamException {
		List<String> lon;
		
		lon = new LinkedList<String>(); String id=null;
		
		try {      // for error handling
			id=net_identifier();
			if ( inputState.guessing==0 ) {
				lon.add(id);
			}
			{
			_loop22:
			do {
				if ((LA(1)==COMMA)) {
					match(COMMA);
					id=net_identifier();
					if ( inputState.guessing==0 ) {
						lon.add(id);
					}
				}
				else {
					break _loop22;
				}
				
			} while (true);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_4);
			} else {
			  throw ex;
			}
		}
		return lon;
	}
	
	public final String  net_identifier() throws RecognitionException, TokenStreamException {
		String nm;
		
		nm=null;
		
		try {      // for error handling
			nm=identifier();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_9);
			} else {
			  throw ex;
			}
		}
		return nm;
	}
	
	public final Range  constant_range_expression() throws RecognitionException, TokenStreamException {
		Range rng;
		
		rng=null; Vnumber lb=null, rb=null;
		
		try {      // for error handling
			lb=number();
			match(COLON);
			rb=number();
			if ( inputState.guessing==0 ) {
				rng = new Range(lb,rb);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_10);
			} else {
			  throw ex;
			}
		}
		return rng;
	}
	
	public final void parameter_instance() throws RecognitionException, TokenStreamException {
		
		
		try {      // for error handling
			match(POUND);
			match(LPAREN);
			parameter_instance_ele();
			{
			_loop34:
			do {
				if ((LA(1)==COMMA)) {
					match(COMMA);
					parameter_instance_ele();
				}
				else {
					break _loop34;
				}
				
			} while (true);
			}
			match(RPAREN);
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_7);
			} else {
			  throw ex;
			}
		}
	}
	
	public final void module_instance(
		Module mod, String ref
	) throws RecognitionException, TokenStreamException {
		
		String inst=null; Cell cell=null;
		
		try {      // for error handling
			inst=module_instance_identifier();
			if ( inputState.guessing==0 ) {
				cell = mod.createCell(ref,inst);
			}
			match(LPAREN);
			{
			switch ( LA(1)) {
			case LPAREN:
			case DOT:
			case IDENT:
			case LCURLY:
			case NUMBER:
			case ESCAPED_IDENT:
			{
				list_of_port_connections(cell);
				break;
			}
			case RPAREN:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			match(RPAREN);
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_11);
			} else {
			  throw ex;
			}
		}
	}
	
	public final String  module_instance_identifier() throws RecognitionException, TokenStreamException {
		String nm;
		
		nm=null;
		
		try {      // for error handling
			nm=identifier();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_12);
			} else {
			  throw ex;
			}
		}
		return nm;
	}
	
	public final void list_of_port_connections(
		Cell cell
	) throws RecognitionException, TokenStreamException {
		
		PortConnection pc=null;
		
		try {      // for error handling
			switch ( LA(1)) {
			case LPAREN:
			case IDENT:
			case LCURLY:
			case NUMBER:
			case ESCAPED_IDENT:
			{
				pc=ordered_port_connection();
				if ( inputState.guessing==0 ) {
					cell.addConn(pc);
				}
				{
				_loop38:
				do {
					if ((LA(1)==COMMA)) {
						match(COMMA);
						pc=ordered_port_connection();
						if ( inputState.guessing==0 ) {
							cell.addConn(pc);
						}
					}
					else {
						break _loop38;
					}
					
				} while (true);
				}
				break;
			}
			case DOT:
			{
				pc=named_port_connection();
				if ( inputState.guessing==0 ) {
					cell.addConn(pc);
				}
				{
				_loop40:
				do {
					if ((LA(1)==COMMA)) {
						match(COMMA);
						pc=named_port_connection();
						if ( inputState.guessing==0 ) {
							cell.addConn(pc);
						}
					}
					else {
						break _loop40;
					}
					
				} while (true);
				}
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_13);
			} else {
			  throw ex;
			}
		}
	}
	
	public final void parameter_instance_ele() throws RecognitionException, TokenStreamException {
		
		Expression unused=null;
		
		try {      // for error handling
			switch ( LA(1)) {
			case DOT:
			{
				match(DOT);
				match(IDENT);
				match(LPAREN);
				unused=expression();
				match(RPAREN);
				break;
			}
			case LPAREN:
			case IDENT:
			case LCURLY:
			case NUMBER:
			case ESCAPED_IDENT:
			{
				unused=expression();
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_14);
			} else {
			  throw ex;
			}
		}
	}
	
	public final Expression  expression() throws RecognitionException, TokenStreamException {
		Expression expr;
		
		expr=null; Primary pr=null;
		
		try {      // for error handling
			pr=primary();
			if ( inputState.guessing==0 ) {
				expr = new Expression(pr);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_15);
			} else {
			  throw ex;
			}
		}
		return expr;
	}
	
	public final PortConnection  ordered_port_connection() throws RecognitionException, TokenStreamException {
		PortConnection pc;
		
		Expression expr=null; pc=null;
		
		try {      // for error handling
			expr=expression();
			if ( inputState.guessing==0 ) {
				pc = new PortConnection(expr);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_14);
			} else {
			  throw ex;
			}
		}
		return pc;
	}
	
	public final PortConnection  named_port_connection() throws RecognitionException, TokenStreamException {
		PortConnection pc;
		
		String id=null; Expression expr=null; pc=null;
		
		try {      // for error handling
			match(DOT);
			id=port_identifier();
			{
			switch ( LA(1)) {
			case LPAREN:
			{
				match(LPAREN);
				{
				switch ( LA(1)) {
				case LPAREN:
				case IDENT:
				case LCURLY:
				case NUMBER:
				case ESCAPED_IDENT:
				{
					expr=expression();
					break;
				}
				case RPAREN:
				{
					break;
				}
				default:
				{
					throw new NoViableAltException(LT(1), getFilename());
				}
				}
				}
				match(RPAREN);
				break;
			}
			case RPAREN:
			case COMMA:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			if ( inputState.guessing==0 ) {
				pc = new PortConnection(id,expr);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_14);
			} else {
			  throw ex;
			}
		}
		return pc;
	}
	
	public final Concat  concatenation() throws RecognitionException, TokenStreamException {
		Concat cc;
		
		cc=null; Expression expr=null;
		List<Expression> exprs = new LinkedList<Expression>();
		
		try {      // for error handling
			match(LCURLY);
			expr=expression();
			if ( inputState.guessing==0 ) {
				exprs.add(expr);
			}
			{
			_loop47:
			do {
				if ((LA(1)==COMMA)) {
					match(COMMA);
					expr=expression();
					if ( inputState.guessing==0 ) {
						exprs.add(expr);
					}
				}
				else {
					break _loop47;
				}
				
			} while (true);
			}
			match(RCURLY);
			if ( inputState.guessing==0 ) {
				cc = new Concat(exprs);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_15);
			} else {
			  throw ex;
			}
		}
		return cc;
	}
	
	public final MultipleConcat  multiple_concatenation() throws RecognitionException, TokenStreamException {
		MultipleConcat mc;
		
		mc=null; Expression expr=null; Concat cc=null;
		
		try {      // for error handling
			match(LCURLY);
			expr=constant_expression();
			cc=concatenation();
			match(RCURLY);
			if ( inputState.guessing==0 ) {
				mc = new MultipleConcat(expr,cc);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_15);
			} else {
			  throw ex;
			}
		}
		return mc;
	}
	
	public final Expression  constant_expression() throws RecognitionException, TokenStreamException {
		Expression expr;
		
		expr=null;
		
		try {      // for error handling
			expr=expression();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_16);
			} else {
			  throw ex;
			}
		}
		return expr;
	}
	
	public final Vnumber  number() throws RecognitionException, TokenStreamException {
		Vnumber n;
		
		Token  id = null;
		n=null;
		
		try {      // for error handling
			id = LT(1);
			match(NUMBER);
			if ( inputState.guessing==0 ) {
				n = new Vnumber(id.getText());
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_17);
			} else {
			  throw ex;
			}
		}
		return n;
	}
	
	public final Range  part_select() throws RecognitionException, TokenStreamException {
		Range rng;
		
		rng=null; Vnumber lb=null, rb=null;
		
		try {      // for error handling
			match(LBRACK);
			lb=number();
			{
			switch ( LA(1)) {
			case COLON:
			{
				match(COLON);
				rb=number();
				break;
			}
			case RBRACK:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			match(RBRACK);
			if ( inputState.guessing==0 ) {
				rng = new Range(lb,rb);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_18);
			} else {
			  throw ex;
			}
		}
		return rng;
	}
	
	public final Primary  primary() throws RecognitionException, TokenStreamException {
		Primary prim;
		
		prim=null; Vnumber vn=null; String id=null; Range rng=null;
		MultipleConcat mc=null; Concat cc=null; Expression expr=null;
		
		try {      // for error handling
			switch ( LA(1)) {
			case NUMBER:
			{
				vn=number();
				if ( inputState.guessing==0 ) {
					prim = new Primary(vn);
				}
				break;
			}
			case IDENT:
			case ESCAPED_IDENT:
			{
				id=identifier();
				{
				switch ( LA(1)) {
				case LBRACK:
				{
					rng=part_select();
					break;
				}
				case SEMI:
				case RPAREN:
				case COMMA:
				case LCURLY:
				case RCURLY:
				{
					break;
				}
				default:
				{
					throw new NoViableAltException(LT(1), getFilename());
				}
				}
				}
				if ( inputState.guessing==0 ) {
					prim = new Primary(id,rng);
				}
				break;
			}
			case LPAREN:
			{
				match(LPAREN);
				expr=expression();
				match(RPAREN);
				if ( inputState.guessing==0 ) {
					prim = new Primary(expr);
				}
				break;
			}
			default:
				boolean synPredMatched60 = false;
				if (((LA(1)==LCURLY) && (_tokenSet_19.member(LA(2))))) {
					int _m60 = mark();
					synPredMatched60 = true;
					inputState.guessing++;
					try {
						{
						multiple_concatenation();
						}
					}
					catch (RecognitionException pe) {
						synPredMatched60 = false;
					}
					rewind(_m60);
inputState.guessing--;
				}
				if ( synPredMatched60 ) {
					mc=multiple_concatenation();
					if ( inputState.guessing==0 ) {
						prim = new Primary(mc);
					}
				}
				else if ((LA(1)==LCURLY) && (_tokenSet_19.member(LA(2)))) {
					cc=concatenation();
					if ( inputState.guessing==0 ) {
						prim = new Primary(cc);
					}
				}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_15);
			} else {
			  throw ex;
			}
		}
		return prim;
	}
	
	public final String  identifier() throws RecognitionException, TokenStreamException {
		String nm;
		
		Token  id1 = null;
		Token  id2 = null;
		nm=null;
		
		try {      // for error handling
			switch ( LA(1)) {
			case IDENT:
			{
				id1 = LT(1);
				match(IDENT);
				if ( inputState.guessing==0 ) {
					nm = id1.getText();
				}
				break;
			}
			case ESCAPED_IDENT:
			{
				id2 = LT(1);
				match(ESCAPED_IDENT);
				if ( inputState.guessing==0 ) {
					nm = id2.getText();
				}
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				recover(ex,_tokenSet_20);
			} else {
			  throw ex;
			}
		}
		return nm;
	}
	
	
	public static final String[] _tokenNames = {
		"<0>",
		"EOF",
		"<2>",
		"NULL_TREE_LOOKAHEAD",
		"\"module\"",
		"SEMI",
		"\"endmodule\"",
		"LPAREN",
		"RPAREN",
		"\"inout\"",
		"\"input\"",
		"\"output\"",
		"COMMA",
		"\"wire\"",
		"LBRACK",
		"RBRACK",
		"POUND",
		"DOT",
		"IDENT",
		"LCURLY",
		"RCURLY",
		"COLON",
		"\"assign\"",
		"ASSIGN",
		"NUMBER",
		"ESCAPED_IDENT",
		"\"and\"",
		"\"nand\"",
		"\"nor\"",
		"\"or\"",
		"\"xor\"",
		"\"xnor\"",
		"SIZED_NUMBER",
		"SIZE",
		"BASE",
		"SIZED_DIGIT",
		"UNSIZED_NUMBER",
		"DIGIT",
		"HEXDIGIT",
		"WS2",
		"WS",
		"CNTRL",
		"SL_COMMENT",
		"ML_COMMENT"
	};
	
	private static final long[] mk_tokenSet_0() {
		long[] data = { 2L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_0 = new BitSet(mk_tokenSet_0());
	private static final long[] mk_tokenSet_1() {
		long[] data = { 18L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_1 = new BitSet(mk_tokenSet_1());
	private static final long[] mk_tokenSet_2() {
		long[] data = { 4265881088L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_2 = new BitSet(mk_tokenSet_2());
	private static final long[] mk_tokenSet_3() {
		long[] data = { 33882272L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_3 = new BitSet(mk_tokenSet_3());
	private static final long[] mk_tokenSet_4() {
		long[] data = { 32L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_4 = new BitSet(mk_tokenSet_4());
	private static final long[] mk_tokenSet_5() {
		long[] data = { 4265881152L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_5 = new BitSet(mk_tokenSet_5());
	private static final long[] mk_tokenSet_6() {
		long[] data = { 288L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_6 = new BitSet(mk_tokenSet_6());
	private static final long[] mk_tokenSet_7() {
		long[] data = { 33816576L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_7 = new BitSet(mk_tokenSet_7());
	private static final long[] mk_tokenSet_8() {
		long[] data = { 4512L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_8 = new BitSet(mk_tokenSet_8());
	private static final long[] mk_tokenSet_9() {
		long[] data = { 8409120L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_9 = new BitSet(mk_tokenSet_9());
	private static final long[] mk_tokenSet_10() {
		long[] data = { 32768L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_10 = new BitSet(mk_tokenSet_10());
	private static final long[] mk_tokenSet_11() {
		long[] data = { 4128L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_11 = new BitSet(mk_tokenSet_11());
	private static final long[] mk_tokenSet_12() {
		long[] data = { 128L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_12 = new BitSet(mk_tokenSet_12());
	private static final long[] mk_tokenSet_13() {
		long[] data = { 256L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_13 = new BitSet(mk_tokenSet_13());
	private static final long[] mk_tokenSet_14() {
		long[] data = { 4352L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_14 = new BitSet(mk_tokenSet_14());
	private static final long[] mk_tokenSet_15() {
		long[] data = { 1577248L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_15 = new BitSet(mk_tokenSet_15());
	private static final long[] mk_tokenSet_16() {
		long[] data = { 524288L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_16 = new BitSet(mk_tokenSet_16());
	private static final long[] mk_tokenSet_17() {
		long[] data = { 3707168L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_17 = new BitSet(mk_tokenSet_17());
	private static final long[] mk_tokenSet_18() {
		long[] data = { 9965856L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_18 = new BitSet(mk_tokenSet_18());
	private static final long[] mk_tokenSet_19() {
		long[] data = { 51118208L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_19 = new BitSet(mk_tokenSet_19());
	private static final long[] mk_tokenSet_20() {
		long[] data = { 43864480L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_20 = new BitSet(mk_tokenSet_20());
	
	}
