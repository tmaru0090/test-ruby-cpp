


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





# グローバル変数
$player_x = 400
$player_y = 500
$player_speed = 5
$player_image_key = ""
$font_key = ""
$meteor_image_key = ""
$meteors = []
$meteor_speed = 1.5
$game_over = false
$countdown = 5
$countdown_font_key = ""
$start_time = Time.now

# 隕石を生成する関数
def generate_meteor
  meteor = {
    x: rand(0..800),
    y: -50,
    size: 50
  }
  $meteors << meteor
end

def init
  # ウィンドウサイズを設定
  set_window_size(800, 600)

  # 画像とフォントのロード
  #$player_image_key = load_image("write-san.jpg")
  $font_key = load_font("Corporate-Logo-Bold-ver3.otf")
  $meteor_image_key = load_image("zoom-kisida.jpg")
  $countdown_font_key = load_font("Corporate-Logo-Bold-ver3.otf")
end

def update
  return if $game_over

  # カウントダウンの更新
  if $countdown > 0
    elapsed_time = Time.now - $start_time
    if elapsed_time >= 1
      $countdown -= 1
      $start_time = Time.now
    end
    return
  end

  # キー入力に基づいてプレイヤーを移動
  if is_key_pressed(KEY_UP)
    $player_y -= $player_speed
  end
  if is_key_pressed(KEY_DOWN)
    $player_y += $player_speed
  end
  if is_key_pressed(KEY_LEFT)
    $player_x -= $player_speed
  end
  if is_key_pressed(KEY_RIGHT)
    $player_x += $player_speed
  end

  # 隕石の位置を更新
  $meteors.each do |meteor|
    meteor[:y] += $meteor_speed

    # プレイヤーとの衝突判定
    if ($player_x - meteor[:x]).abs < meteor[:size] && ($player_y - meteor[:y]).abs < meteor[:size]
      $game_over = true
      set_end_flag(true)
      puts "Game Over! You were hit by a meteor."
    end
  end

  # 画面外に出た隕石を削除し、新しい隕石を生成
  $meteors.reject! { |meteor| meteor[:y] > 600 }
  generate_meteor if rand(100) < 5
end

def draw
  # カウントダウンが終了していない場合、カウントダウンを表示
  if $countdown > 0
    draw_text(350, 250, "#{$countdown}", $countdown_font_key, 50)
    return
  end

  # プレイヤー画像を描画
  draw_text($player_x, $player_y,"--Player--",$font_key,10)

  # 隕石を描画
  $meteors.each do |meteor|
    draw_image($meteor_image_key, meteor[:x], meteor[:y], 0.0)
  end

  # ゲームオーバーのテキストを表示
  if $game_over
    draw_text(300, 250, "Game Over", $font_key, 50)
  end
end

# ゲーム開始
