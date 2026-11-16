import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import java.io.*;

public class DumpDecomp extends GhidraScript {
    public void run() throws Exception {
        String out=getScriptArgs()[0];
        DecompInterface di=new DecompInterface();
        di.openProgram(currentProgram);
        PrintWriter pw=new PrintWriter(new FileWriter(out));
        PrintWriter idx=new PrintWriter(new FileWriter(out+".index"));
        for(Function f: currentProgram.getFunctionManager().getFunctions(true)){
            DecompileResults r=di.decompileFunction(f,120,monitor);
            long sz=f.getBody().getNumAddresses();
            idx.printf("%s %6d %s%n",f.getEntryPoint(),sz,f.getName());
            pw.printf("// ===== %s @ %s (size %d)%n",f.getName(),f.getEntryPoint(),sz);
            if(r!=null&&r.decompileCompleted()) pw.println(r.getDecompiledFunction().getC());
            else pw.println("// decompile failed");
        }
        pw.close();idx.close();
    }
}
