/****************************************************************************************
 *	P.Y. copyright	 			                                                        *
 *	Game Main        		   	                                                        *
 *	Status: progressing        	                                                        *
 *	init -> (shootable)pressed -> released(unshootable) -> shoot ->  (shootable)pressed *
 ***************************************************************************************/
package game;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.ResourceBundle;

import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.image.ImageView;
import javafx.scene.input.KeyCode;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.Pane;
import javafx.scene.shape.Line;
import javafx.scene.text.Font;
import javafx.stage.Stage;
import javafx.util.Duration;

public class GameController implements Initializable {
    private final double _X = 195;
    private final double _Y = 520;
    private static final int SPEED = 5;

    private double mouseX = 195; // default: (_X, _Y)
    private double mouseY = 520;
    private double blockBottom = 0; // y max = 696 + BLOCK_WIDTH
    private Boolean mousePressing = false; // true if mouse pressing time
    private Boolean mousePressed = false; // true if mouse pressed on grid
    private Boolean ballMoving = false;
    private Boolean firstBall = false;
    private byte blockMovingCount = 10;
    private byte times = 1;

    private LinkedList<Ball> _balls = new LinkedList<Ball>();
    private LinkedList<Block> _blocks = new LinkedList<Block>();

    private Line shootLine;

    @FXML
    Pane _field;
    @FXML
    GridPane _player_plane;
    @FXML
    ImageView _ball;
    @FXML
    ImageView _pause;
    @FXML
    Label _level;
    @FXML
    Label _suggestion;
    @FXML
    ImageView _mouse;
    @FXML
    Button _square;
    @FXML
    Button _speed;

    @Override
    public void initialize(URL arg0, ResourceBundle arg1) {
        shootLine = new Line();
        _field.getChildren().add(shootLine);
        shootLine.setVisible(false);
        Global.level = 0;
        _level.setText("Level 1");
        _ball.setLayoutX(188);
        _ball.setLayoutY(748);
        getBlock();
        mousePressing = true;
        _mouse.setVisible(true);
        _square.setVisible(false);
        _suggestion.setVisible(true);

        Timeline fps = new Timeline(new KeyFrame(Duration.millis(1000 / 60), (e) -> {
            // mousePressing
            if (mousePressing) {
                double x = mouseX - _mouse.getFitWidth() / 2;
                double y = mouseY - _mouse.getFitHeight() / 2;
                if (x < 49) {
                    x = 49;
                } else if (x > 337) {
                    x = 337;
                }
                if (y > 747) {
                    y = 747;
                }
                _mouse.setLayoutX(x);
                _mouse.setLayoutY(y);
            }

            // block moving
            ArrayList<Block> tBlocks = new ArrayList<Block>(_blocks);
            if (blockMovingCount < 5) {
                double tmpBottom = 0;
                for (var b : tBlocks) {
                    tmpBottom = Math.max(tmpBottom, b.getPosi_y());
                    // System.out.println(" tmpBottom:" + tmpBottom);
                    // System.out.println(" run block");
                    b.movePosi_y(10);
                }
                blockBottom = tmpBottom;

                if (blockBottom >= 696) { // == 696
                    blockMovingCount = 5;
                    endGame();
                }
                blockMovingCount++;
            }

            // ball moving
            ArrayList<Ball> tBalls = new ArrayList<Ball>(_balls); // TODO: PUT THIS IN TIMELINE SO THAT IT CAN BE SHOW
                                                                  // UP
            for (byte i = times; i > 0; --i) {
                for (var b : tBalls) {
                    // System.out.println("run ball");
                    // System.out.println("Before ball.posi_x:" + b.getPosi_x() + " ball.posi_y:" +
                    // b.getPosi_y());
                    b.movePosi(SPEED * b.getDirX(), SPEED * b.getDirY());
                    // System.out.println("After ball.posi_x:" + b.getPosi_x() + " ball.posi_y:" +
                    // b.getPosi_y());
                    for (var bl : tBlocks) {
                        bl.minusNum(b);
                        if (bl.getNum() == 0) {
                            _field.getChildren().remove(bl.getBTN());
                            _blocks.remove(bl);
                        }
                    }
                    if (b.getPosi_x() > 328 || b.getPosi_x() < 49) {
                        b.revertX();
                    }
                    if (b.getPosi_y() < 49) {
                        b.revertY();
                    }
                    if (b.getPosi_y() > 748) {
                        if (firstBall) {
                            firstBall = false;
                            _ball.setLayoutX(b.getPosi_x());
                            _ball.setLayoutY(748);
                            _ball.setVisible(true);
                        }
                        _balls.remove(b); // remove List
                        _field.getChildren().remove(b.imageView); // remove clone
                    }
                    // TODO: touch block reflect::IMPLEMENT IN Block's sons(and Ball also)
                }
            }
            if (ballMoving && tBalls.size() == 0) {
                ballMoving = false;
                nextLevel();
            }
        }));
        fps.setCycleCount(Timeline.INDEFINITE);
        fps.play();
    }

    // ****************************************************************************************************
    // TODO: use key WSAD... to decide which point to point is the vector to shoot
    // _level.layout(163, 9)
    @FXML // unimplement
    public void onPausePressed() throws IOException { // open menu // TODO: open menu at the same stage. Making sure
                                                      // that the data in GameController won't lost'
        Parent menu = FXMLLoader.load(getClass().getResource("ControlMenuScene.fxml"));
        Scene menuScene = new Scene(menu);
        menuScene.getRoot().requestFocus();
        BBTANGENTController.currStage.setScene(menuScene);
    }

    @FXML
    public void shoot() {
        // mousePressed = false;
        // System.out.println("work in shoot");

        // shoot
        shootLine.setVisible(false);
        _ball.setVisible(false);
        firstBall = true;
        for (int i = Global.level; i > 0; --i) {
            ImageView newIV = new ImageView(_ball.getImage());
            newIV.setLayoutX(_ball.getLayoutX());
            newIV.setLayoutY(_ball.getLayoutY());
            newIV.setFitHeight(_ball.getFitHeight());
            newIV.setFitWidth(_ball.getFitWidth());
            // System.out.println("create ball");
            // // System.out.println("mouseY:" + mouseY + " _ball.getLayoutY():" +
            // _ball.getLayoutY());
            // // System.out.println("Ball mouseY - _ball.getLayoutY():" + (mouseY -
            // _ball.getLayoutY()));
            _balls.push(new Ball(newIV, mouseX - _ball.getLayoutX(), mouseY - _ball.getLayoutY()));
            _field.getChildren().add(newIV);
        }
        ballMoving = true;
        /*
         * ball.setDirection(mouseX - ball.getPosi_x(), mouseY - ball.getPosi_y());
         * while (true) { // abstract ball move // System.out.println("abstract ball~");
         * //// System.out.println("ball x:" + ball.getPosi_x() + " y:" + //
         * ball.getPosi_y()); //// System.out.println("ball dirX:" + ball.getDirX() +
         * " dirY:" + // ball.getDirY()); ball.movePosi(SPEED * ball.getDirX(), SPEED *
         * ball.getDirY()); if (ball.getPosi_x() > 328 || ball.getPosi_y() < 48) { //
         * System.out.println("+revX"); ball.revertX(); } if (ball.getPosi_y() < 48) {
         * ball.revertY(); // System.out.println("+revY"); } if (ball.getPosi_y() > 748)
         * { break; } } ball.imageView.setLayoutY(748);
         * _ball.setLayoutX(ball.getPosi_x()); _ball.setLayoutY(ball.getPosi_y());
         */
    }

    @FXML
    public void onMousePressed(MouseEvent m) {
        _suggestion.setVisible(false);
        if (mousePressing) {
            // System.out.println("Mouse pressed on Grid");
            mousePressed = true;
        } else {
            // System.out.println("mousePressing is false");
        }
    }

    @FXML
    public void onMouseDragged(MouseEvent m) {
        _suggestion.setVisible(false);
        if (mousePressed = true) {
            mouseX = m.getX();
            mouseY = m.getY();

            shootLine.setStartX(_ball.getLayoutX() + _ball.getFitWidth()/2);
            shootLine.setStartY(_ball.getLayoutY() + _ball.getFitHeight()/2);
            double x = mouseX - _mouse.getFitWidth() / 2;
            double y = mouseY - _mouse.getFitHeight() / 2;
            if (x < 49) {
                x = 49;
            } else if (x > 337) {
                x = 337;
            }
            if (y > 747) {
                y = 747;
            }
            shootLine.setEndX(x + _mouse.getFitWidth() / 2);
            shootLine.setEndY(y + _mouse.getFitHeight() / 2);

            shootLine.toFront();
            shootLine.setVisible(true);
        }
        else{
            shootLine.setVisible(false);
        }
    }

    @FXML
    public void onMouseReleased(MouseEvent m) {
        if (mousePressed == true) {
            // System.out.println("Mouse released on Pane");
            mouseX = m.getX();
            mouseY = m.getY();
            if (m.getY() > 747) {

            } else {
                mousePressing = false;
                mousePressed = false;
                _mouse.setVisible(false);
                // System.out.println("X:" + mouseX + " Y:" + mouseY);
                shoot();
            }
        }
    }

    @FXML
    public void onMouseTimesPressed() {
        if (times == 1) {
            times = 5;
            _speed.setText(">> 1");
        } else { // times == 5
            times = 1;
            _speed.setText(">> 5");
        }
    }

    // ****************************************************************************************************
    private void storePlayData(String playerName) {
        int score = Global.level;
        File playerData = new File("playerData.txt");
        // delete file
        if (playerData.delete()) {
            // System.out.println("Deleted the file: " + playerData.getName());
        } else {
            // System.out.println("Failed to delete the file.");
        }
        // recreate file
        try {
            if (playerData.createNewFile()) {
                // System.out.println("File created: " + playerData.getName());
            } else {
                // System.out.println("File already exists.");
            }
        } catch (IOException e) {
            // System.out.println("An error occurred.");
            e.printStackTrace();
        }
        // write file
        try {
            FileWriter dataWriter = new FileWriter("playerData.txt");
            dataWriter.write("highestScorePlayer: " + playerName + "\n");
            dataWriter.write("highestScore: " + score);
            Global.storeHSName(playerName);
            Global.storeHS(score);
            dataWriter.close();

            // System.out.println("Successfully wrote to the file.");
        } catch (IOException e) {
            // System.out.println("An error occurred.");
            e.printStackTrace();
        }
    }

    private void getBlock() {
        // no block 0.3 // square 0.4 // triangle 0.3
        // System.out.println("get block");
        double x = 52, y = 46;
        if (Global.level == 0) {
            y = 96;
        }
        Global.level++;
        for (byte i = 6; i > 0; --i) {
            double exist = Math.random();
            if (exist >= 0.5) {
                // byte type = (byte)(Math.random()*10); // TODO: TYPE TRIANGLE
                Button b = new Button();
                b.setLayoutX(x);
                b.setLayoutY(y);
                b.setCancelButton(true);
                b.setStyle(_square.getStyle());
                b.setPrefSize(50, 50);
                Square s = new Square(b);
                b.setText(String.valueOf(s.getNum()));
                _blocks.push(s);
                _field.getChildren().add(b);
            }
            x += 50;
        }
    }

    private void endGame() {
        Global.level--;
        if (Global.level > Global.getHS()) {
            // Parent name = FXMLLoader.load(getClass().getResource("NameInputScene.fxml"));

            Pane namePane = new Pane();
            Label scoreLabel = new Label();
            TextField nameField = new TextField();
            Button nameBTN = new Button("Submit");

            scoreLabel.setLayoutX(67.5 - ((int) (Math.log(Global.level) / Math.log(10))) * 6.5);
            scoreLabel.setLayoutY(34);
            scoreLabel.setFont(Font.font(24));
            scoreLabel.setText("Best Score: " + String.valueOf(Global.level));
            scoreLabel.setVisible(true);
            nameField.setPromptText("Please Enter Your Name");
            nameField.setLayoutX(61);
            nameField.setLayoutY(85);
            nameBTN.setLayoutX(108);
            nameBTN.setLayoutY(124);

            namePane.getChildren().addAll(scoreLabel, nameField, nameBTN);
            Scene nameScene = new Scene(namePane, 270, 170);

            Stage nameStage = new Stage();
            nameStage.setResizable(false);
            nameStage.setTitle("BEST SCORE!!!");
            nameStage.setScene(nameScene);
            nameStage.show();

            nameField.setOnKeyPressed(e -> {
                String playerName = "#no_name#";
                if (e.getCode() == KeyCode.ENTER) {
                    String tmp = nameField.getText();
                    // System.out.println(tmp.length());
                    if (!tmp.equals("")) {
                        playerName = tmp;
                    }
                    storePlayData(playerName);
                    Global.storeHS(Global.level);
                    Global.storeHSName(playerName);
                    openEndPane(true);
                    nameStage.close();
                }
            });
            nameBTN.setOnAction(e -> {
                String playerName = "#no_name#";
                String tmp = nameField.getText();
                // System.out.println(tmp.length());
                if (!tmp.equals("")) {
                    playerName = tmp;
                }
                storePlayData(playerName);
                Global.storeHS(Global.level);
                Global.storeHSName(playerName);
                openEndPane(true);
                nameStage.close();
            });
        } else {
            openEndPane(false);
        }
    }

    private void openEndPane(Boolean win) {
        // ConclusionScene.fxml
        Pane endPane = new Pane();
        Label message = new Label();
        Button turnMenu = new Button("Back To Menu");

        message.setFont(Font.font(24));
        if (win) {
            message.setText("You Win");
            message.setLayoutX(90);
            message.setLayoutY(43);
        } else {
            message.setText("You Loss");
            message.setLayoutX(88);
            message.setLayoutY(43);
        }
        message.setVisible(true);
        turnMenu.setLayoutX(89);
        turnMenu.setLayoutY(101);

        endPane.getChildren().addAll(message, turnMenu);
        Scene endScene = new Scene(endPane, 270, 170);

        Stage endStage = new Stage();
        endStage.setResizable(false);
        endStage.setTitle("Game End");
        endStage.setScene(endScene);
        endStage.show();

        turnMenu.setOnAction(e -> {
            try {
                Parent root = FXMLLoader.load(getClass().getResource("StartScene.fxml"));
                Scene startScene = new Scene(root);

                // BBTANGENTController.startScene.getRoot().requestFocus();
                BBTANGENTController.currStage.setScene(startScene);
                endStage.close();
            } catch (IOException e1) {
                e1.printStackTrace();
            }
        });
    }

    private void nextLevel() {
        // // System.out.println("run ball finished");
        // move down blocks
        blockMovingCount = 0;
        getBlock(); // include Global.level++

        // touch bottom end game
        // // System.out.println("block bottomest:" + blockBottom);

        // next level
        mouseX = _X;
        mouseY = _Y;
        _level.setLayoutX(173 - (int) Global.level / 10 * 5);
        _level.setText("Level " + String.valueOf(Global.level));
        mousePressing = true;
        _mouse.setVisible(true);
        _ball.toFront();
        _mouse.toFront();
    }
}