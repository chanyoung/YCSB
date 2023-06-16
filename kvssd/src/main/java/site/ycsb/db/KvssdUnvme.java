package site.ycsb.db;

/**
 * Java binding for unvme.
 */
public class KvssdUnvme {
  static {
    System.loadLibrary("KvssdUnvme");
  }

  public native int init(String configPath);
  public native int destroy();
  public native int insert(int devIdx, String key, String value);
  public native int scan(int devIdx, int prefix, int recordcount);
}

