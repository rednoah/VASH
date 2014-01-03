function download_cc_web_video()
% http://vireo.cs.cityu.edu.hk/webvideo/videos/QueryID/VideoName    
server_videoPath = 'http://vireo.cs.cityu.edu.hk/webvideo/videos/';
storagePath = 'cc_web_video/videos/';

[VideoID QueryID Source VideoName URL]=textread('Video_List.txt','%s%s%s%s%s','delimiter','\t');

for i=1:length(VideoID)
    if (~exist([storagePath QueryID{i}],'dir'))
        mkdir([storagePath QueryID{i}]);
    end
    % error handling when url doesn't exist.
    try
        urlwrite([server_videoPath QueryID{i} '/' VideoName{i}],[storagePath QueryID{i} '/' VideoName{i}]);
    catch
        %display i and VideoName of the file fail to download
        i
        VideoName{i}
        f=fopen([storagePath 'missing_file.txt'],'at+');
        fprintf(f,'%s is missing!\r\n',VideoName{i});
        fclose(f);
        continue
    end
end

end