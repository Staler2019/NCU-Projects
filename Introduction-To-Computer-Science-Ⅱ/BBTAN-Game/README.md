# 計算機概論 期末專題 BBTANGENT

## Content

參考 111% 公司出品的「BBTAN」，重製一個不包含功能球的致敬作品。前期只有打算製作方格下降的類型。

## Author

name: 楊佳峻
class: B

## Framework

| Package | Class |
| ------- | ------------------------------- |
| Game | GameController(proc)<br/>DifficultyController(fin)<br/>BBTANGENTController(fin)<br/>Ball(fin)<br/>Block(proc)<br/>Global |
---
| Class | SubClass |
| ----- | ---------------------- |
| Block | Square<br/>Triangle |
---
(undone modifying)
| Class | Member |
| ----- | ------ |
| BBTANGENTController | -public static Stage currStage<br/>-public static Scene startScene<br/>-@FXML private AnchorPane _start_pane<br/>-@FXML private Label _tap_word<br/>-@FXML private Button _info_btn<br/>-@FXML private GridPane _score_pane<br/>-@FXML private Label _title<br/>-@FXML private Label _scoreboard<br/><br/>+public static void main(String[] args)<br/><br/>+@Override public void initialize(URL arg0, ResourceBundle arg1)<br/>+@Override public void start(Stage primaryStage)<br/>+@FXML public void onStartPressed()<br/>+@FXML public void checkEnterPressed(KeyEvent k)<br/>+@FXML public void onInfoPressed()<br/>+@FXML public void onHyperPressed()[UNIMPLEMENTED]<br/>***problem: controllerClass, press link to outside browser |
| DifficultyController | -@FXML private Button diff_1<br/>-@FXML private Button diff_2<br/>-@FXML private Button diff_3<br/><br/>+@Override public void initialize(URL arg0, ResourceBundle arg1)<br/>+public void turnToGame()<br/>+@FXML public void diff_1Pressed()<br/>+@FXML public void diff_2Pressed()[UNIMPLEMENTED]<br/>+@FXML public void diff_3Pressed()[UNIMPLEMENTED] |
| GameController |  |
| ControllerMenuSceneController | +@FXML public void onResumePressed()<br/>+@FXML public void onReturnToMenuPressed() |
| Ball | -private double dir_x<br/>-private double dir_y<br/>-public ImageView imageView<br/><br/>+public Ball(ImageView image, double dirX, double dirY)<br/><br/>+public void setDirection(double x, double y)<br/>+public double getDirX()<br/>+public double getDirY()<br/>+public double revertX()<br/>+public double revertY()<br/>+public double getPosi_x()<br/>+public double getPosi_y()<br/>+public void movePosi_x(double x)<br/>+public void movePosi_y(double y)<br/>+public void movePosi(double x, double y) |
| Block | -protected int num<br/>-protected Button button<br/><br/>+public Block(Button btn)<br/><br/>+public int getNum()<br/>+public double getPosi_x()<br/>+public double getPosi_y()<br/>+public void nextLevel()<br/>+public abstract void minusNum(Ball b) |
| +Square | +public Square(Button btn)<br/><br/>+@Override public void minusNum(Ball b) |
| +Triangle | -private byte direction<br/><br/>+public Triangle(Button btn)<br/><br/>+@Override public void minusNum(Ball b) |
| Global |  |

## Sources

1. pause icon(simpleicon.com)
