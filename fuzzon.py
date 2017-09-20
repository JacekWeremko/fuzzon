import os
import argparse
from bs4 import builder


def run_blocking(args, poll_stdout = False, outfile_path = None):
    import subprocess   
    import unicodedata
    
    proc = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    
    decoding = "utf-16"
    stdout = ""
    stderr = ""
    if (poll_stdout is True):
        import sys
        while True:
            line = proc.stdout.readline()
            if line == b'' and proc.poll() is not None:
#             if line == b'' and proc.poll() is not  0:
                break
#             
            write_me = str(line)
            write_me = write_me[2:-1]
            write_me = write_me[0:-2]
            write_me = write_me + '\r\n'
             
#             line = line.decode("utf-8")
#             stdout += line.decode(decoding)
            stdout += write_me
#             print(str(line))
            sys.stdout.write(write_me)
            sys.stdout.flush()
            
        
    pstdout, pstderr = proc.communicate()  
#     stdout = stdout + pstdout.decode(decoding)
#     stderr = stderr + pstderr.decode(decoding)
    stdout = stdout + str(pstdout)
    stderr = stderr + str(pstderr)
    if (outfile_path is not None):
        with open(outfile_path, 'a+') as output_file:
            if (len(stdout) != 0):
                output_file.write(stdout)
            if (len(stderr) != 0):
                output_file.write("STRERR:")
                output_file.write(stderr)

    if(proc.returncode != 0):
        print("args: " + str(args))
        print("return_code: " + str(proc.returncode))
        
        if (len(stdout) != 0):
            print("stdout: " + stdout)
        if (len(stderr) != 0):
            print("stderr: " + stderr)   
    return proc.returncode

class Builder(object):
#     CALNG_PATH = "clang"
    CALNG_PATH = "clang++-4.0"
    OUTPUT_FOLDER_NAME = "fuzzon_out"

      
    def build_dir(self, source_dir, fuzzonlib, cflags, lflags, mode):
        output_files = []
        if (source_dir is not None) : 
            for file in os.listdir(source_dir):
                if file.endswith(".cpp"):
                    new_source_file = os.path.join(source_dir, file)
                    output_files.append(self.build_single(new_source_file, None, fuzzonlib, cflags, lflags, mode))
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
        return output_file

    def _change_main_name(self, obj_file): 
        main_func_name = "main"

    def _compile(self, source_file, cflags):     
        file_name_wo_ext = os.path.splitext(source_file)[0]
        dir_name = os.path.join(os.path.dirname(source_file), self.OUTPUT_FOLDER_NAME)
        file_name = os.path.basename(file_name_wo_ext)
        
        if not os.path.exists(dir_name):
            os.makedirs(dir_name)
        obj_file = os.path.join(dir_name, file_name + ".bc")
            
            
        if (cflags is None):
            cflags = []
            cflags.append("-O3")
#             cflags.append("-g3")
            cflags.append("-Wall")
            cflags.append("-fmessage-length=0")
#             cflags.append("-fno-exceptions")
#             cflags.append("-std=c++1y")
            


        cflags.append("-S")
        cflags.append("-emit-llvm")
        cflags.append("-fsanitize-coverage=edge,trace-pc-guard")
        cflags.append("-fsanitize-coverage=trace-cmp")
#         cflags.append("-fsanitize-coverage=trace-gep")
        
         
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
        if (output_file is None):
            output_file = os.path.splitext(obj_file)[0]
            
        if (lflags is None):
            lflags = []
            lflags.append("-L/usr/lib/gcc/x86_64-linux-gnu/6/")
            lflags.append("-L/usr/lib/x86_64-linux-gnu/")
            lflags.append("-L" + fuzzonlib)
            
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
    
class Tester(object):
    FUZZON_NAME = "fuzzon"
    
    def __init__(self, fuzzonexe):
        import datetime
        
        if (fuzzonexe is None):
            self.fuzzonexe = self.FUZZON_NAME
        else:  
            self.fuzzonexe = fuzzonexe
            
        self.start_test_datatime = str(datetime.datetime.now()).replace(" ", "_")
        self.predefined_options = {
            "touch" : ["--generate", 25] +   ["--mutate_d", 0] + ["--mutate_nd", 100]    + ["--total_timeout", 1000 * 60 * 1],
            "medium": ["--generate", 1000] + ["--mutate_d", 0] + ["--mutate_nd", 1000]   + ["--total_timeout", 1000 * 60 * 5],
            "full"  : ["--generate", 2500] + ["--mutate_d", 1] + ["--mutate_nd", 100000] + ["--total_timeout", 1000 * 60 * 30]
        } 
        
    def _get_basic_options(self, level):
        if (level is None):
            level = "default"
        
        options = []
        options.extend(["-vvvv"])
        if (level == "touch"):
            options.extend(self.predefined_options["touch"]);
        if (level == "medium"):
            options.extend(self.predefined_options["medium"]);
        elif (level == "full"):  
            options.extend(self.predefined_options["full"]);
        options.extend(["--single_test_timeout", 50])
        options.extend(["--total_testcases", 1000000])
        return options
    
    def _output_base_dir(self, sut):
        sut_basepath = os.path.dirname(sut)
        output_base = os.path.join(sut_basepath, 
#                                    self.FUZZON_NAME + "_out", 
                                    "test_results",
                                   self.start_test_datatime)
        os.makedirs(output_base, exist_ok=True)
        return output_base
    
    def _output_sut_dir(self, sut):
        sut_exe_name = os.path.basename(sut)    
        output_base = os.path.join(self._output_base_dir(sut), sut_exe_name)
        os.makedirs(output_base, exist_ok=True)
        return output_base
                
    def test_single(self, sut, input_format, level, additional_options, iterations):
        if (additional_options is None):
            additional_options = [str(elem) for elem in self._get_basic_options(level)]
    
        # this is agrh ;(
        summary_file_path = os.path.join(self._output_base_dir(sut), "summary.txt")
        
        per_sut_additional_options = []
        per_sut_additional_options.extend(additional_options)
        per_sut_additional_options.extend(["--out", self._output_sut_dir(sut)])  
        
        args = []
        args.append(self.fuzzonexe)
        args.extend(["--sut", sut])
        args.extend(["--input_format", input_format])
        args.extend(per_sut_additional_options)
        
        print("Fuzzon testing start: " + str(args))
        for i in range(0, iterations):
            retcode = run_blocking(args, True, summary_file_path)
            print("Finished! retcode :" + str(retcode))
        print("Fuzzon testing end.")   
        return 
    
    def test_campaign(self, sut_list, input_format, level, additional_options, iterations):
        if (additional_options is None):
            additional_options = [str(elem) for elem in self._get_basic_options(level)]
    
        sut_list = sorted(sut_list)
        print("Fuzzon campaign start")
        for sut in sut_list:
            self.test_single(sut, input_format, level, additional_options, iterations)
        print("Fuzzon campaign end.")      
        return 
#     
#     def test_campaign(self, sut_list, input_format, level, additional_options, iterations):
#         if (additional_options is None):
#             additional_options = [str(elem) for elem in self._get_basic_options(level)]
#         
# 
#         for sut in sut_list:
#             args = []
#             args.append(fuzzonexe)
#             args.extend(["--sut", sut])
#             args.extend(["--input_format", input_format])
#             
#             sut_basepath = os.path.dirname(sut)
#             sut_exe_name = os.path.basename(sut)
#             
#             output_base = os.path.join(sut_basepath, (FUZZON_NAME + "_" + sut_exe_name), str(datetime.datetime.now()).replace(" ", "_"))
#             additional_options.extend(["--out", output_base]) 
#             additional_options.extend(["--corpus_seeds", os.path.join(output_base, "corpus")])   
#             
#             
#             args.extend(additional_options)
#             
#             print("Fuzzon campaign start: " + str(args))
#             for i in range(0, iterations):
#                 retcode = run_blocking(args, True)
#                 print("Finished! retcode :" + str(retcode))
#             print("Fuzzon campaign end.")   
#         return 



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
    
    
    group_test = parser.add_argument_group("test", description="Testing options.")
    group_test.add_argument("--test_single",    dest='test_single',action='store_true',  required=False, help="Simple tests.")
    group_test.add_argument("--test_campaign",  dest='test_campaign',action='store_true',  required=False, help="Testing campaign.")
    group_test.add_argument("--iterations",     type=int,   action='store',       required=False, help="Number of testing iterations.")
    group_test.add_argument("--fuzzonexe",      type=str,   action='store',       required=False, help="Path to fuzzon executable.")
    group_test.add_argument("--sut",            type=str,   action='store',       required=False, help="Path to software under test.")
    group_test.add_argument("--input_format",   type=str,   action='store',       required=False, help="Path to SUT input format file.")
    group_test.add_argument("--level",          type=str,   action='store',       required=False, help="Select testing level - touch, default, full")
    

    args = parser.parse_args()
    print(args)
    

    if (args.mode == "build_single"):
        builder = Builder()
        sut_list = [builder.build_single(args.source_file, args.output_file, args.fuzzonlib, args.cflag, args.lflag, args.execution_mode)]
    elif (args.mode == "build_dir"):
        builder = Builder()
        sut_list = builder.build_dir(args.source_dir, args.fuzzonlib, args.cflag, args.lflag, args.execution_mode)
    

    
    additional_options = None
    if (args.test_single):
        tester = Tester( args.fuzzonexe)
        tester.test_single(sut_list[0], args.input_format, args.level, additional_options, args.iterations)
    if (args.test_campaign):
        tester = Tester( args.fuzzonexe)
        tester.test_campaign(sut_list, args.input_format, args.level, additional_options, args.iterations)
    elif (args.sut) :        
        tester = Tester( args.fuzzonexe)
        tester.test_single(args.sut, args.input_format, args.level, additional_options, args.iterations)
        
    print("Finish.")
    return
    

if __name__ == "__main__":
    main()
    