package pack;

import java.io.IOException;
import java.io.OutputStream;

/**
 * UNUSED AT THE MOMENT
 * @author Administrator
 *
 */
public class SerialWriter implements Runnable {

	private OutputStream out;

	public SerialWriter(OutputStream out) {
		this.out = out;
	}

	public void run() {
		try {
			int c = 0;
			while ((c = System.in.read()) > -1) {
				this.out.write(c);
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
