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
package nldb;

import java.io.*;
import java.util.LinkedList;
import java.util.zip.GZIPInputStream;
import nldb.parser.*;
import static nldb.Util.info;
import static nldb.Util.getErrorMsgCnt;
import static nldb.Util.warn;

/**
 *
 * @author karl
 */
public class Load {

    private static int m_loadCnt = 0;
    
    /**
     * Read netlists.
     * @param fnames netlist file names.
     * @return 0 or error message count (during read).
     */
    public static int readNetlists(String fnames[]) {
        int mcnt = getErrorMsgCnt();
        Load unused = new Load(fnames);
        mcnt = getErrorMsgCnt() - mcnt;
        return mcnt;
    }

    /**
     * Load netlists.
     * @param fnames netlist filenames.
     */
    private Load(String fnames[]) {
        this(null, fnames, 0);
    }
    
    public Load(String top, String argv[], int link) {
        if (0 < m_loadCnt++) {
            Module.reset();
        }
        String linkStr = System.getProperty("link");
        if (null != linkStr) {
            link = Integer.parseInt(linkStr);
        }
        load(top, argv, link);
    }

    public Load(String top, String argv[]) {
        this(top, argv, 2);
    }

    public Module getMod() {
        return m_top;
    }

    public static void main(String argv[]) {
        String top = argv[0];
        String fname = argv[1];
        LinkedList<String> files = new LinkedList<String>();
        try {
            BufferedReader fis = new BufferedReader(new FileReader(fname));
            while (fis.ready()) {
                files.add(fis.readLine());
            }
            fis.close();
        } catch (Exception ex) {
            nldb.Util.error(ex);
        }
        String fargs[] = files.toArray(new String[0]);
        Load load = new Load(top, fargs);
    }
    private Module m_top = null;

    private void load(String top, String argv[], int link) {
        long time, elapsed = System.currentTimeMillis();
        try {
            String fn = null;
            VnlLexer lexer = null;
            for (int i = 0; i < argv.length; i++) {
                if (argv[i].startsWith("-")) {
                    continue;
                } else if (0 < i) {
                    Module.setMarkAsLibcell(argv[i - 1].equals("--lib"));
                }
                InputStream ins = null;
                InputStreamReader insRdr = null;
                fn = argv[i];
                if (! new File(fn).canRead()) {
                    warn("FILE-2", fn, "read");
                    continue;
                }
                if (fn.endsWith(".gz")) {
                    ins = new GZIPInputStream(new FileInputStream(fn));
                    lexer = new VnlLexer(ins);
                } else {
                    insRdr = new FileReader(fn);
                    lexer = new VnlLexer(insRdr);
                }
                lexer.setFilename(fn);
                VnlParser parser = new VnlParser(lexer);
                parser.setFilename(fn);
                time = System.currentTimeMillis();
                parser.start();
                time = System.currentTimeMillis() - time;
                time /= 1000; //to secs
                //String msg = (Module.getMarkAsLibcell()) ? "(as a library) " : "";
                info("TIM-1", fn, time);
                if (null != ins) {
                    ins.close();
                } else {
                    insRdr.close();
                }
            }
            if (null != top) {
                m_top = Module.link(top, 10, link);
                if (false) {
                    String ofn = top + ".dump.txt";
                    info("FILE-1", ofn);
                    PrintStream pos = new PrintStream(new FileOutputStream(ofn));
                    m_top.print(pos);
                    pos.close();
                }
            }
        } catch (Exception ex) {
            ex.printStackTrace();
            System.exit(1);
        }
        elapsed = System.currentTimeMillis() - elapsed;
        elapsed /= 1000; //to secs
        info("TIM-2", elapsed);
    }
}
