import argparse
from bs4 import builder




class Builder(object):
    CALNG_PATH = "clang++-4.0"
               
    def build(self, source_file, output_file, fuzzonlib, cflags, lflags):
        import os
        import sys
        import subprocess
        
        obj_file = self._compile(source_file, cflags) 
        print("obj_file : " + obj_file)
        
        output_file = self._link(obj_file, output_file, fuzzonlib, lflags)
        print("output_file : " + output_file)
        return

    def _compile(self, source_file, cflags):
        import os
        import subprocess
        
        obj_file = os.path.splitext(source_file)[0] + ".bc"
            
            
        if (cflags is None):
            cflags = []
            cflags.append("-O0")
            cflags.append("-g3")
            cflags.append("-Wall")
            cflags.append("-c")
            cflags.append("-fmessage-length=0")
            cflags.append("-std=c++1y")
         
        cflags.append("-emit-llvm")
        cflags.append("-fsanitize-coverage=edge,trace-pc-guard")
         
        args = []
        args.append(self.CALNG_PATH)
        args += cflags
        args.append("-o")
        args.append(obj_file)
        args.append(source_file)
         
        compiler_process = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = compiler_process.communicate()
        print("args: " + str(args))
        print("stdout: " + str(stdout))
        print("stderr: " + str(stderr))
        return obj_file
        
    
    def _link(self, obj_file, output_file, fuzzonlib, lflags):
        import os
        import sys
        import subprocess
        
        if (output_file is None):
            output_file = os.path.splitext(obj_file)[0]
            
        if (lflags is None):
            lflags = []
            lflags.append("-L/usr/lib/gcc/x86_64-linux-gnu/6/")
            lflags.append("-L/usr/lib/x86_64-linux-gnu/")
            lflags.append("-L" + fuzzonlib[0])
            
            lflags += ["-lpthread", "-lrt", "-lboost_log_setup", "-lboost_log", "-lboost_system", "-lboost_thread"]
    #         lflags.append("-lstdc++")
    #         lflags.append("-lfuzzon")
    
        args = []
        args.append(self.CALNG_PATH)
        args += lflags
        args.append("-o")
        args.append(output_file)
        args.append(obj_file)
        args.append("-lstdc++")
        args.append("-lfuzzon")
         
        print("args: " + str(args))
        compiler_process = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = compiler_process.communicate()
        print("stdout: " + str(stdout))
        print("stderr: " + str(stderr))
        return output_file


def generate():
    return

def test():
    FUZZON_PATH = "fuzzon"
    
    return 




def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("mode", help='Select mode', nargs='?', choices=("build", "test"))
#     root_group = parser.add_mutually_exclusive_group(required=True);
#     root_group.add_argument("build", type=str, help="buil",default=None, action="store", required=False)
#     root_group.add_argument("test", type=str, help="test",default=None, action="store", required=False)
    
    group_build = parser.add_argument_group('build', description="Build your project")
    group_build.add_argument('--source_file', type=str, action='store',  required=False, help="Path to source file. Single file support only.") # required
    group_build.add_argument('--output_file', type=str, action='store',  required=False, help="Name of output file.")
    group_build.add_argument('--fuzzonlib',   type=str, action='append', required=False, help="Directory where libfuzoon.a is placed")
    group_build.add_argument('--cflag',       type=str, action='append', required=False, help="Additional compilation flags you want to use. ")   
    group_build.add_argument('--lflag',       type=str, action='append', required=False, help="Additional linking flags you want to use. ")
    
    
    group_test = parser.add_argument_group("test", description="Start testing")
    group_test.add_argument("--sut",    type=str, action='store',  required=False, help="Path to software under test.") # required
    group_test.add_argument("--config", type=str, action='store',  required=False, help="Path to fuzzer configuration file.")      
    

    args = parser.parse_args()
    print(args)
    
    if (args.mode == "build"):
        builder = Builder()
        builder.build(args.source_file, args.output_file, args.fuzzonlib, args.cflag, args.lflag)
    if (args.mode == "test"):
        test()
        
    
    
    print("Finish.")
    return
    

    

if __name__ == "__main__":
    main()
    