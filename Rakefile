require 'rake/clean'
require 'rake/loaders/makefile'

# ARDUINO = "/Applications/Arduino/1.0/Arduino.app/Contents/Resources/Java"
# CXX = "#{ARDUINO}/hardware/tools/avr/bin/avr-g++"
# AR = "#{ARDUINO}/hardware/tools/avr/bin/avr-ar"
# CXX_FLAGS = "-Os -finline-functions -Wall -Wno-strict-aliasing -Wno-inline -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -Werror=return-type -mmcu=atmega328p -DF_CPU=16000000UL -DBT_PF_AVR -DARDUINO=100 -DNDEBUG"
# INCLUDES = "-I#{ARDUINO}/hardware/arduino/cores/arduino -I#{ARDUINO}/hardware/arduino/variants/standard -I#{ARDUINO}/libraries/SPI"    


ARDUINO = "/Applications/Arduino/1.5/Arduino.app/Contents/Java"
            
CXX = "#{ARDUINO}/hardware/tools/avr/bin/avr-g++"
AR = "#{ARDUINO}/hardware/tools/avr/bin/avr-ar"
CXX_FLAGS = "-Os -finline-functions -Wall -Wno-strict-aliasing -Wno-inline -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -Werror=return-type -mmcu=atmega328p -DF_CPU=16000000UL -DBT_PF_AVR -DARDUINO=100 -DNDEBUG"
INCLUDES = "-I#{ARDUINO}/hardware/arduino/avr/cores/arduino -I#{ARDUINO}/hardware/arduino/avr/variants/standard -I#{ARDUINO}/hardware/arduino/avr/libraries/SPI"    


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
