public class Tianyu {
	public static int getInt(int n) {
		return n + 10;
	}

	public static void printStudent(Student stu) {
		if (stu != null) {
			System.out.println(stu.toString());
		}
	}

	public static class Student {
		public int id;
		public String name;
		public int age;
		public Student() {
			id = 00001;
			name = "tianyu";
			age = 18;
		}
		public String toString() {
			return "id:" + id + " name:" + name + " age:" + age;
		}
	}
}