package tests.t1;
import static nldb.jni.TclBridge.execute;

/**
 *
 * @author gburdell
 */
public class Test2 {
    public static void main(String argv[]) {
        Object rv;
        rv = execute(new String[] {"get_tool_version"});
        rv = execute(new String[] {"read_verilog", "m2.gv"});
    }
}
