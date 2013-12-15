// filebot -script grab-i-frames.groovy --output /path/to/frames /path/to/videos
def output = new File(_args.output).getCanonicalFile()

args.getFiles{ f -> f.isVideo() }.each{ f ->
    println "[FILE] $f"
    def outputTemplate = new File(new File(output, f.nameWithoutExtension), 'frame-%04d.png')
    outputTemplate.parentFile.mkdirs()
    
    // -vf expr -> only include I-frames
    // -r 1 -> 1 frame per second
    execute('ffmpeg', '-i', f, '-vf', /select="eq(pict_type\,I)"/, '-vsync', '2', '-f', 'image2', outputTemplate)
}
