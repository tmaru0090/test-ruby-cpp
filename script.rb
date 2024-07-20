


#def play_music_sequence(directory, base_name, start_num, end_num)
#  music = Music.new
#  (start_num..end_num).each do |i|
#    file_path = "#{directory}/#{base_name}_#{"%02d" % i}.mp3"
#    music.music_play(file_path)
#  end
#end
#content =File.read('example.txt')
#puts content
 
# 例として、01から10までの連番ファイルを再生する
#directory = "/mnt/c/Users/tanukimaru/Downloads/irisu203/irisu203/data/bgm/"
#base_name = "irisu"
#start_num = 4
#end_num = 6
#m = Music.new 
#m.music_play_sequence(directory, base_name, start_num, end_num)

def init
  #music_play("pokemon-bw-y-sentou.wav")
end
def update
end
def draw
  draw_image("zoom-kisida.jpg")
end


