

package tests.t1;
import  nldb.*;

/**
 *
 * @author gburdell
 */
public class Test1 {
    public static void main(String argv[]) {
        String top = argv[0];
        String nargv[] = new String[argv.length-1];
        System.arraycopy(argv, 1, nargv, 0, nargv.length);
        final int link = 0;
        Module mod = (new Load(top, nargv, link)).getMod();
        //boolean ok = mod.deleteCell("dff1_reg");
        Save unused = new Save(mod, mod.getName()+".save.gv");
    }
}
