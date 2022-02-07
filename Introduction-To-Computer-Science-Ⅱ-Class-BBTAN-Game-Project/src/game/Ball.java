/********************************
 *	P.Y. copyright	 			*
 *	Class Ball        		   	*
 *	Status: finished        	*
 ********************************/
package game;

import javafx.scene.image.ImageView;

public class Ball { // x: 48 ~ 298

    private double dir_x, dir_y;
    public ImageView imageView;

    public Ball(ImageView imageview, double dirX, double dirY) { // x between 45 to 330; y between 45 to 750
        this.imageView = imageview;
        double length = Math.sqrt(Math.pow(dirX, 2) + Math.pow(dirY, 2));
        this.dir_x = dirX / length;
        this.dir_y = dirY / length;
    }

    public void setDirection(double x, double y) {
        double length = Math.sqrt(Math.pow(x, 2) + Math.pow(y, 2));
        this.dir_x = x / length;
        this.dir_y = y / length;
    }

    public double getDirX() {
        return this.dir_x;
    }

    public double getDirY() {
        return this.dir_y;
    }

    public void revertX() {
        this.dir_x *= (-1);
    }

    public void revertY() {
        this.dir_y *= (-1);
    }

    public void revertAtVertex(double dirX, double dirY) {
        this.dir_x = 2 * dirX - this.dir_x;
        this.revertX();
        this.dir_y = 2 * dirY - this.dir_y;
        this.revertY();
    }

    public void revertAtVertex(Square s) { // happen in friction
        double dirX, dirY;
        if (s.getPosi_x() - this.getPosi_x() < 0) { // top
            if (s.getPosi_y() - this.getPosi_y() < 0) { // left
                dirX = Math.sqrt(2) / 2;
                dirY = Math.sqrt(2) / 2;
            } else { // s.getPosi_y() - this.getPosi_y() < 0 -> right
                dirX = -Math.sqrt(2) / 2;
                dirY = Math.sqrt(2) / 2;
            }
        } else { // s.getPosi_x - this.getPosi_x > 0 -> bottom
            if (s.getPosi_y() - this.getPosi_y() < 0) { // left
                dirX = Math.sqrt(2) / 2;
                dirY = -Math.sqrt(2) / 2;
            } else { // s.getPosi_y() - this.getPosi_y() < 0 -> right
                dirX = -Math.sqrt(2) / 2;
                dirY = -Math.sqrt(2) / 2;
            }
        }
        this.revertAtVertex(dirX, dirY);
    }

    public double getPosi_x() {
        return this.imageView.getLayoutX();
    }

    public double getPosi_y() {
        return this.imageView.getLayoutY();
    }

    // public void setPosi(double posiX, double posiY) {
    // , double posiX, double posiY
    // }

    public void movePosi_x(double x) {
        this.imageView.setLayoutX(this.imageView.getLayoutX() + x);
    }

    public void movePosi_y(double y) {
        this.imageView.setLayoutY(this.imageView.getLayoutY() + y);
    }

    public void movePosi(double x, double y) {
        this.movePosi_x(x);
        this.movePosi_y(y);
    }
}