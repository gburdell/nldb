/*#
 #  The MIT License
 # 
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
 #*/
package nldb.jni;

import java.util.Arrays;
import java.util.HashMap;
import static nldb.Util.invariant;
import nldb.jni.TclBridge.Rval;

/**
 * The singleton object used by all commands.
 *
 * @author gburdell
 */
public class Command {

    public static interface ICommand {

        public Rval command(Object argv[]);
    }

    public Command() {
        init();
    }

    public static Rval runCommand(String cmd, Object argv[]) {
        ICommand icmd = stTheOne.m_commandByName.get(cmd);
        invariant(null != icmd);
        //drop subcommand from [0]
        final int n = argv.length;
        if (1 < n) {
            argv = Arrays.copyOfRange(argv, 1, argv.length);
        } else {
            argv = new Object[0];
        }
        Rval r = icmd.command(argv);
        return r;
    }

    private void init() {
        invariant(null == stTheOne);
        stTheOne = this;
        m_commandByName = new HashMap<String, ICommand>();
        //add commands
        m_commandByName.put("get_tool_version", new CommandAtoG.GetToolVersion());
        m_commandByName.put("read_verilog", new CommandHtoZ.ReadVerilog());
    }
    private static Command stTheOne = new Command();
    private HashMap<String, ICommand> m_commandByName;
}
