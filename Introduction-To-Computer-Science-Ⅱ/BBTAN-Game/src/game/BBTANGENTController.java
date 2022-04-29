/********************************
 *	P.Y. copyright	 	    	*
 *	Game Loader         	   	*
 *	Status: finished   		   	*
 ********************************/
package game;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.NoSuchElementException;
import java.util.ResourceBundle;
import java.util.Scanner;

import javafx.application.Application;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
//import javafx.scene.control.Hyperlink; //TODO: hyperlink isn't implemented'
import javafx.scene.control.Label;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.GridPane;
import javafx.stage.Stage;


public class BBTANGENTController extends Application implements Initializable {
    public static Stage currStage;
    public Scene startScene;

    @FXML
    private AnchorPane _start_pane;
    @FXML
    private Label _tap_word; // unused
    @FXML
    private Button _info_btn; // unused
    @FXML
    private GridPane _score_pane;
    @FXML
    private Label _title; // unused
    @FXML
    private Label _scoreboard;

    // @FXML private Hyperlink _info_hyper;


    public static void main(String[] args) {
        // readPlayHistory();
        try {
            File playerData = new File("playerData.txt");
            // check if created
            if (playerData.createNewFile()) {
                System.out.println("File created: " + playerData.getName());
            } else {
                System.out.println("File already exists.");
                // read data
                Scanner dataReader = new Scanner(playerData);
                String a = dataReader.nextLine();
                if (!a.equals("") && !a.equals("highestScorePlayer: ")) {
                    Global.storeHSName(a.split(" ")[1]);
                    a = dataReader.nextLine();
                    Global.storeHS(Integer.valueOf(a.split(" ")[1]));
                }
                dataReader.close();
            }
        } catch (IOException e) {
            System.out.println("An error occurred.");
            e.printStackTrace();
        } catch (NoSuchElementException n) {
            System.out.println("The file is empty.");
            n.printStackTrace();
        }


        launch(args);
    }

	@Override
	public void initialize(URL arg0, ResourceBundle arg1) {

		String scoreText = "Highest: ";
        int scoreTextLonger = 0;
        if(Global.getHSName().equals("")){
            _scoreboard.setVisible(false);
            _score_pane.setVisible(false);
        }
        else {
            scoreText += String.valueOf(Global.getHS()) + ", " + Global.getHSName();
            scoreTextLonger = scoreText.length() - 20;
            //_score_pane.setLayoutX(102 - 5 * scoreTextLonger);
            //_score_pane.setPrefWidth(195 + 10 * scoreTextLonger);
            _scoreboard.setLayoutX(113 - 5 * scoreTextLonger);
            _scoreboard.setText(scoreText);
            _scoreboard.setVisible(true);
            _score_pane.setVisible(true);
        }
    }

    @Override
    public void start(Stage primaryStage) throws Exception {
        currStage = primaryStage;
        Parent root = FXMLLoader.load(getClass().getResource("StartScene.fxml"));
        startScene = new Scene(root);

        currStage.setResizable(false);
        currStage.setTitle("BBTANGENT");
        startScene.getRoot().requestFocus();
        currStage.setScene(startScene);
        currStage.show();
    }

    // ****************************************************************
    @FXML
    public void onStartPressed() throws IOException { // select difficulty
        Parent diff = FXMLLoader.load(getClass().getResource("DifficultyScene.fxml"));
        Scene diffScene = new Scene(diff);
        diffScene.getRoot().requestFocus();
        currStage.setScene(diffScene);
    }

    @FXML
    public void checkEnterPressed(KeyEvent k) throws IOException {
        if (k.getCode() == KeyCode.ENTER) {
            onStartPressed();
        } else if (k.getCode() == KeyCode.F1) {
            Global.setDifficulty(1);

            Parent game = FXMLLoader.load(getClass().getResource("GameScene.fxml"));
            Scene gameScene = new Scene(game);
            gameScene.getRoot().requestFocus();
            BBTANGENTController.currStage.setScene(gameScene);
        }
    }

    @FXML
    public void onInfoPressed() throws IOException {
        Parent info = FXMLLoader.load(getClass().getResource("InfoScene.fxml"));
        Scene infoScene = new Scene(info);

        Stage infoStage = new Stage();
        infoStage.setResizable(false);
        infoStage.setTitle("Info of Game");
        infoStage.setScene(infoScene);
        infoStage.show();

        currStage.getScene().getRoot().requestFocus();
    }


    // @FXML
    // public void onHyperPressed() {}

}