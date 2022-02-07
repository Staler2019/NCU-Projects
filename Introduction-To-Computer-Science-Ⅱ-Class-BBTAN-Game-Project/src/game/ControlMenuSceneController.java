/********************************
 *	P.Y. copyright	 	    	*
 *	In Game Menu         	   	*
 *	Status: finished          	*
 ********************************/
package game;

import java.io.IOException;

import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;

public class ControlMenuSceneController {

    @FXML
    public void onResumePressed() throws IOException {
        Parent game = FXMLLoader.load(getClass().getResource("GameScene.fxml"));
        Scene gameScene = new Scene(game);
        gameScene.getRoot().requestFocus();
        BBTANGENTController.currStage.setScene(gameScene);
    }

    @FXML
    public void onReturnToMenuPressed() throws IOException {
        Parent root = FXMLLoader.load(getClass().getResource("StartScene.fxml"));
        Scene startScene = new Scene(root);
        startScene.getRoot().requestFocus();
        BBTANGENTController.currStage.setScene(startScene);
    }
}