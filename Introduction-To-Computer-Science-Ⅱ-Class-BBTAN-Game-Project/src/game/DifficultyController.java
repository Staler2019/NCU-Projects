/********************************
 *	P.Y. copyright	 			*
 *	Difficulty Selector        	*
 *	Status: finished        	*
 ********************************/
package game;

import java.io.IOException;
import java.net.URL;
import java.util.ResourceBundle;

import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;

public class DifficultyController implements Initializable{

    @FXML private Button diff_1; // unused
    @FXML private Button diff_2;
    @FXML private Button diff_3;

    @Override
    public void initialize(URL arg0, ResourceBundle arg1) { // TODO: difficulty system isn't implemented
        diff_2.setDisable(true);
        diff_3.setDisable(true);
    }

    public void turnToGame() throws IOException {
        Parent game = FXMLLoader.load(getClass().getResource("GameScene.fxml"));
        Scene gameScene = new Scene(game);
        gameScene.getRoot().requestFocus();
        BBTANGENTController.currStage.setScene(gameScene);
    }

    @FXML
    public void diff_1Pressed() throws IOException {
        Global.setDifficulty(1);
        turnToGame();
    }

    @FXML
    public void diff_2Pressed() throws IOException {
        Global.setDifficulty(2);
        turnToGame();
    }

    @FXML
    public void diff_3Pressed() throws IOException {
        Global.setDifficulty(3);
        turnToGame();
    }

}