new File('Video_List.txt').splitEachLine('\t', 'UTF-8') { line ->
	def queryId = line[1]
	def filename = line[3]
	
	def file = new File("$queryId/$filename")
	def url = new URL("http://vireo.cs.cityu.edu.hk/webvideo/videos/$queryId/$filename")

	
	if (file.exists()) {
		println "[SKIPPED] $file"
	} else {
		println url.saveAs(file)
	}
}
