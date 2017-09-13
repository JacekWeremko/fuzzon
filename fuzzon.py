import argparse
from bs4 import builder
# from test.pyston/e import nargs


def run_blocking(args, poll_stdout = False):
    import subprocess   
    
    proc = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    
    
    if (poll_stdout is True):
        import sys
        while True:
            line = proc.stdout.readline()
#             if line == '' and proc.poll() is not None:
            if line == b'' and proc.poll() == 0:
                break
            sys.stdout.write(line.decode("utf-8"))
            sys.stdout.flush()

        
    stdout, stderr = proc.communicate()  
    if(proc.returncode != 0):
        print("args: " + str(args))
        print("return_code: " + str(proc.returncode))
        
        if (len(stdout) != 0):
            print("stdout: " + stdout.decode("utf-8"))
        if (len(stderr) != 0):
            print("stderr: " + stderr.decode("utf-8"))   
    return proc.returncode

class Builder(object):
#     CALNG_PATH = "clang"
    CALNG_PATH = "clang++-4.0"
    OUTPUT_FOLDER_NAME = "fuzzon_out"

      
    def build_dir(self, source_dir, fuzzonlib, cflags, lflags, mode):
        import os
        
        output_files = []
        if (source_dir is not None) : 
            for file in os.listdir(source_dir):
                if file.endswith(".cpp"):
                    new_source_file = os.path.join(source_dir, file)
                    output_files += self.build_single(new_source_file, None, fuzzonlib, cflags, lflags, mode)
        return output_files
    
    def build_single(self, source_file, output_file, fuzzonlib, cflags, lflags, mode):    
        retcode, obj_file = self._compile(source_file, cflags)
        if (retcode != 0):
            return
#         print("obj_file : " + obj_file)
  
        if(mode is None or mode == "process"):
            retcode, output_file = self._link(obj_file, output_file, fuzzonlib, lflags)
        elif (mode == "thread"):
            retcode, output_file = self._linkful(obj_file, output_file, fuzzonlib, lflags)
        else :
            raise "Mode not suported"
        
        if (retcode != 0):
            return
#         print("output_file : " + output_file)
        return [output_file]

    def _change_main_name(self, obj_file): 
        main_func_name = "main"

    def _compile(self, source_file, cflags):
        import os
        
        file_name_wo_ext = os.path.splitext(source_file)[0]
        dir_name = os.path.join(os.path.dirname(source_file), self.OUTPUT_FOLDER_NAME)
        file_name = os.path.basename(file_name_wo_ext)
        
        if not os.path.exists(dir_name):
            os.makedirs(dir_name)
        obj_file = os.path.join(dir_name, file_name + ".bc")
            
            
        if (cflags is None):
            cflags = []
            cflags.append("-O0")
            cflags.append("-g3")
            cflags.append("-Wall")
            cflags.append("-fmessage-length=0")
#             cflags.append("-fno-exceptions")
#             cflags.append("-std=c++1y")
            


        cflags.append("-S")
        cflags.append("-emit-llvm")
        cflags.append("-fsanitize-coverage=edge,trace-pc-guard")
#         cflags.append("-fsanitize=address")
#         cflags.append("-fno-omit-frame-pointer")
#         cflags.append("-fsanitize-address-use-after-scope")
         
        args = []
        args.append(self.CALNG_PATH)
        args += cflags
        args.append("-o")
        args.append(obj_file)
        args.append(source_file)
         
        print("Compiling source_file :" + source_file)  
        retcode = run_blocking(args)
    
        if(retcode == 0):
            print("Successfully compiled.")            
        return retcode, obj_file
        
    
    def _link(self, obj_file, output_file, fuzzonlib, lflags):
        import os
        
        if (output_file is None):
            output_file = os.path.splitext(obj_file)[0]
            
        if (lflags is None):
            lflags = []
            lflags.append("-L/usr/lib/gcc/x86_64-linux-gnu/6/")
            lflags.append("-L/usr/lib/x86_64-linux-gnu/")
            lflags.append("-L" + fuzzonlib[0])
            
#             lflags += ["-lasan"]
            lflags += ["-lstdc++", "-lm", "-lcrypto"]
            lflags += ["-lpthread", "-lrt"]
            lflags += ["-lfuzzon"]
            lflags += ["-lboost_log_setup", "-lboost_log", "-lboost_system", "-lboost_thread"]
#             lflags += ["/usr/lib/llvm-4.0/lib/clang/4.0.0/lib/linux/libclang_rt.asan-x86_64.so"]
            
    
        args = []
        args.append(self.CALNG_PATH)
        args.append("-o")
        args.append(output_file)
        args.append(obj_file)
        args += lflags

      
        print("Linking obj_file :" + obj_file)  
        retcode = run_blocking(args)
    
        if(retcode == 0):
            print("Successfully linked.")            
        return retcode, output_file
    
    def _linkful(self, obj_file, output_file, fuzzonlib, lflags):
        import os
        
        if (output_file is None):
            output_file = os.path.splitext(obj_file)[0]
            
        if (lflags is None):
            lflags = []
            #lflags += ["--entry_point", "fuzzon_main"]
#             lflags += ["-e", "fuzzon_main"]
            lflags.append("-L/usr/lib/gcc/x86_64-linux-gnu/6/")
            lflags.append("-L/usr/lib/x86_64-linux-gnu/")
            lflags.append("-L" + fuzzonlib[0])
            
#             lflags += ["-lasan"]
            lflags += ["-lstdc++", "-lm", "-lcrypto"]
            lflags += ["-lpthread", "-lrt"]
            lflags += ["-lfuzzon"]
            lflags += ["-lboost_log_setup", "-lboost_log", "-lboost_system", "-lboost_thread"]
            lflags += ["-lboost_program_options", "-lboost_thread", "-lboost_date_time", "-lboost_filesystem"]
            lflags += ["-lboost_exception", "-lboost_regex", "-lboost_serialization", "-lboost_random"]
            lflags += ["-lboost_iostreams", "-lboost_regex", "-lboost_serialization", "-lboost_random"]
            
#             lflags += ["/usr/lib/llvm-4.0/lib/clang/4.0.0/lib/linux/libclang_rt.asan-x86_64.so"]
            
    
        args = []
        args.append(self.CALNG_PATH)
        args.append("-o")
        args.append(output_file)
        args.append(obj_file)
        args += lflags

      
        print("Linking obj_file :" + obj_file)  
        retcode = run_blocking(args)
    
        if(retcode == 0):
            print("Successfully linked.")            
        return retcode, output_file
    

def test(level, sut_list, input_format, additional_options, fuzzon_path, iterations):
    import os
    FUZZON_NAME = "fuzzon"
    
    if (fuzzon_path is None):
        fuzzon_path = "./"
    
    if (level is None):
        level = "default"
            
    if (additional_options is None):
        additional_options = []
        
    additional_options.extend(["-vvvv"])
    if (level == "touch"):
        additional_options.extend(["--generate", 25])
        additional_options.extend(["--mutate_d", 0])
        additional_options.extend(["--mutate_nd", 50])
    elif (level == "full"):   
        additional_options.extend(["--generate", 10000])
        additional_options.extend(["--mutate_d", 1])
        additional_options.extend(["--mutate_nd", -1])

    additional_options.extend(["--single_test_timeout", 20])
    additional_options.extend(["--total_timeout", 1000 * 60 * 5])
    for sut in sut_list:
        args = []
        args.append(os.path.join(fuzzon_path, FUZZON_NAME))
        args.extend(["--sut", sut])
        args.extend(["--input_format", input_format])
        
        additional_options = [str(elem) for elem in additional_options]
        args.extend(additional_options)
        
        print("Fuzzon campaign start: " + str(args))
        for i in range(0, iterations):
            retcode = run_blocking(args, True)
            print("Finished! retcode :" + str(retcode))
        print("Fuzzon campaign end.")   
    return 


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("mode", help='Select mode', nargs='?', choices=("build_single", "build_dir"))
  
    group_build = parser.add_argument_group('build', description="Build your project - generic options.")
    group_build.add_argument('--fuzzonlib',     type=str, action='store',   required=False, help="Directory where libfuzoon.a is placed")
    group_build.add_argument('--cflag',         type=str, action='append',  required=False, help="Additional compilation flags you want to use. ")   
    group_build.add_argument('--lflag',         type=str, action='append',  required=False, help="Additional linking flags you want to use. ")
    group_build.add_argument('--execution_mode',type=str, action='store',   required=False, help="Mode of fuzzer execution: process | thread.")
    
    
    group_build = parser.add_argument_group('build_single', description="Build your project - single file mode.")
    group_build.add_argument('--source_file',   type=str, action='store',  required=False, help="Path to source file. Single file support only.") # required
    group_build.add_argument('--output_file',   type=str, action='store',  required=False, help="Name of output file.")
    
    group_build = parser.add_argument_group('build_dir', description="Build your projects - multiple files mode.")
    group_build.add_argument('--source_dir',    type=str, action='store',  required=False, help="Path to directory with source files. For each cpp file is threaded as separate project.")
    
    
    group_test = parser.add_argument_group("test", description="Start testing")
    group_test.add_argument("--test",           dest='test',action='store_true',  required=False, help="Path to software under test.")
    group_test.add_argument("--iterations",     type=int,   action='store',       required=False, help="Number of testing campaigns.")
    group_test.add_argument("--fuzzon_path",    type=str,   action='store',       required=False, help="Path to fuzzon executable.")
    group_test.add_argument("--sut",            type=str,   action='store',       required=False, help="Path to software under test.")
    group_test.add_argument("--input_format",   type=str,   action='store',       required=False, help="Path to SUT input format file.")
    group_test.add_argument("--level",          type=str,   action='store',       required=False, help="Select testing level - touch, default, full")
    
    #parser.add_argument("test", help='Select testing level', nargs='?', choices=("touch", "default", "full"))      
    

    args = parser.parse_args()
    print(args)
    

    if (args.mode == "build_single"):
        builder = Builder()
        outputs = builder.build_single(args.source_file, args.output_file, args.fuzzonlib, args.cflag, args.lflag, args.execution_mode)
    elif (args.mode == "build_dir"):
        builder = Builder()
        outputs = builder.build_dir(args.source_dir, args.fuzzonlib, args.cflag, args.lflag, args.execution_mode)
    
    
    additional_options = None

    if (args.test):
        test(args.level, outputs, args.input_format, additional_options, args.fuzzon_path)
    elif (args.sut) :        
        outputs = [args.sut]
        test(args.level, outputs, args.input_format, additional_options, args.fuzzon_path)
        
    print("Finish.")
    return
    

    

if __name__ == "__main__":
    main()
    