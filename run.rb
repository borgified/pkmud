#!/usr/bin/env ruby

Dir.chdir("PKarea")
Dir.glob("*.are") do |are_file|
	print "area ", are_file, "\n"
	inside_obj_section = 0
	File.open(are_file,"r").each do |line|
		if line =~ /#OBJECTS/
			inside_obj_section = 1
		end
		if line =~ /^#0$/
			inside_obj_section = 0
		end
		if inside_obj_section == 1
			if line =~ /^#(\d+)/
				print "VNUM: ", $1, "\n"
				$description_count = 0
			end
			if line =~ /^~$/
				next
			end
			if line =~ /(\w.*)~/
				if $description_count == 0
					print "keywords: ", $1, "\n"
				end
				if $description_count == 1
					print "short: ", $1, "\n"
				end
				if $description_count == 2
					print "long: ", $1, "\n"
				end
				$description_count += 1
			end
			if line =~ /^\d+/
				puts line
			end
		end
	end
end

