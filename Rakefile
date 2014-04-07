require 'rake/clean'
require 'rake/loaders/makefile'

CXX = "/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avr-g++"
AR = "/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avr-ar"
CXX_FLAGS = "-Os -finline-functions -Wall -Wno-strict-aliasing -Wno-inline -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -Werror=return-type -mmcu=atmega328p -DF_CPU=16000000UL -DBT_PF_AVR -DARDUINO=100 -DNDEBUG"
INCLUDES = "-I/Applications/Arduino.app/Contents/Resources/Java/hardware/arduino/cores/arduino -I/Applications/Arduino.app/Contents/Resources/Java/hardware/arduino/variants/standard -I/Applications/Arduino.app/Contents/Resources/Java/libraries/SPI"    

TARGET_DIR = 'target'

SRC = FileList['*.cpp']
OBJ = SRC.pathmap("#{TARGET_DIR}/%X.o")
LIB = File.join(TARGET_DIR,"libMqttSn.lib")
   
directory TARGET_DIR

CLEAN.include(TARGET_DIR)  
    
SRC.each do |srcfile|
  objfile = srcfile.pathmap("#{TARGET_DIR}/%X.o") 
  file objfile => [srcfile, TARGET_DIR] do
    sh "#{CXX} -MMD -MT #{objfile} -MF #{objfile}.d #{CXX_FLAGS} #{INCLUDES} -c #{srcfile} -o #{objfile}" 
  end
  
  Rake::MakefileLoader.new.load("#{objfile}.d") if File.file?("#{objfile}.d")
  
end
 
file LIB => OBJ do
  sh "rm -f #{LIB} && #{AR} crs #{LIB} #{OBJ}"
end

task :default => LIB
